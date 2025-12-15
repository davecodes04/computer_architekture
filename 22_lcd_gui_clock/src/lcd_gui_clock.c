#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "lcd/LCD_1in28.h"
#include "gui/gui.h"
#include "math.h"

/*
 * Include the header with an image created from Gimp:
 * A 240x240 pixel image with blue as "green-screen".
 * 
 * We *could* export as .c-File even with RGB565 data,
 * But that would not help much, wed need to copy it
 * byte-for-byte, as well.
 */
#include "../images/homer.h"

#define ERROR(str,err) do {                    \
        fprintf(stderr, "ERROR (%s:%d): %s\n", \
                __FILE__, __LINE__, (str));    \
        exit(err);                             \
    } while(0)

// Main memory for drawing screen, registered to "gui"
static uint16_t * screen;

// 60 end positions on the outskirts of the display
struct pos {
    int x;
    int y;
};
struct pos clock_pos_sec[60] = {0, };
struct pos clock_pos_min[60] = {0, };
struct pos clock_pos_hrs[96] = {0, };

void init_lcd_gui(uint16_t ** screen) {
    uint16_t * image;
    assert (NULL != screen);

    LCD_1IN28_Init(HORIZONTAL);
    DEV_SET_PWM(60); // Required to get the Display to work

    /* Creating a full-sized image -- each Pixel has 2 Bytes*/
    image = malloc (LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT * 2);
    if (NULL == image)
        ERROR("malloc", -1);

    /* Fill it with the color black (no conversion needed) */
    for (int i = 0; i < LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT; i++) {
        image[i] = BLACK;
    }

    gui_image_new((uint8_t*)image, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, ROTATE_0, BLACK);
    // Nice grey background
    for (int r = -1; r < 10; r++) {
        int grey = 10 * abs(r) + 1;
        gui_draw_circle(120, 120, 120 -r, GUI_RGB(grey, grey, grey), DRAW_FILL_EMPTY);
    }
    // Draw 12x the hour line
    for (int r = 0; r < 12; r++) {
        int x1 = 120 + round(108.0 * sin(30.0 * r * M_PI / 180.0));
        int y1 = 120 + round(108.0 * cos(30.0 * r * M_PI / 180.0));
        int x2 = 120 + round(120.0 * sin(30.0 * r * M_PI / 180.0));
        int y2 = 120 + round(120.0 * cos(30.0 * r * M_PI / 180.0));

        gui_draw_line(x1, y1, x2, y2, WHITE);
    }

    // Extract pixels from bitmap-image (8-bit RGB values)
    char * data = header_data;
    char pixel[3];
    for (int i = 0; i < width * height; i++) {
        int row, col;
        HEADER_PIXEL(data, pixel);
        // Skip the blue pixels
        if (pixel[0] == 0x00 && pixel[1] == 0x00 && pixel[2] == 0xff)
            continue;
        row = i / width;
        col = i % width;
        gui_draw_pixel(col, row, GUI_RGB(pixel[0], pixel[1], pixel[2]));
    }

    gui_image_backup();
    *screen = image;
}

void calc_clock_pos(void) {
    for (int r = 0; r < 60; r++) {
        int x = 120 + round(110.0 * cos((360.0/60.0 * r) * M_PI / 180.0));
        int y = 120 + round(110.0 * sin((360.0/60.0 * r) * M_PI / 180.0));
        clock_pos_sec[r].x = x;
        clock_pos_sec[r].y = y;
    }

    for (int r = 0; r < 60; r++) {
        int x = 120 + round(90.0 * cos((360.0/60.0 * r) * M_PI / 180.0));
        int y = 120 + round(90.0 * sin((360.0/60.0 * r) * M_PI / 180.0));
        clock_pos_min[r].x = x;
        clock_pos_min[r].y = y;
    }

    // Even for hours, we split into 96 separate positions
    for (int r = 0; r < 96; r++) {
        int x = 120 + round(70.0 * cos((360.0/96.0 * r) * M_PI / 180.0));
        int y = 120 + round(70.0 * sin((360.0/96.0 * r) * M_PI / 180.0));
        clock_pos_hrs[r].x = x;
        clock_pos_hrs[r].y = y;
    }
}

void new_char_pos(int * x, int * y) {
#define BORDER 10
    static int dx = 5;
    static int dy = 5;
    static int x_inc = 0;
    static int y_inc = 0;
    if (x_inc) {
        if (*x+BORDER < (240-BORDER)) {
            *x += dx;
        } else {
            x_inc = 0;
            dx = rand()%10+1;
        }
    } else {
        if (*x > BORDER) {
            *x -= dx;
        } else {
            x_inc = 1;
            dx = rand()%10+1;
        }
    }

    if (y_inc) {
        if (*y+BORDER < (240-BORDER)) {
            *y += dy;
        } else {
            y_inc = 0;
            dy = rand()%10+1;
        }
    } else {
        if (*y > BORDER) {
            *y -= dy;
        } else {
            y_inc = 1;
            dy = rand()%10+1;
        }
    }
}

int main() {
    if (DEV_Module_Init() != 0)
        ERROR("DEV_Module_Init", -1);

    printf("Creating an image, initializing the screen\n");
    init_lcd_gui (&screen);
    calc_clock_pos();

    /* Additionally show how the duty-cycle of the background-lighting works */
    int x_pos = 120;
    int y_pos = 120;
    int seconds = 0;
    int minutes = 10;
    int hours = 3;
    int count = 0 ;
    while (true) {
        int hours_arr;
        char time[16];
        int time_len;

        gui_image_restore();
        time_len = snprintf(time, sizeof(time), "%d:%d Uhr", hours, minutes);
        time[time_len] = '\0';
        gui_draw_string(x_pos, y_pos, time, &Font24, RED, WHITE);
        new_char_pos(&x_pos, &y_pos);

        // Draw the hours first (in the back)
        hours_arr = hours*8 + (minutes * 16) / 120;
        gui_draw_line_width (120, 120, clock_pos_hrs[(hours_arr + 96-24) % 96].x, clock_pos_hrs[(hours_arr + 96-24) % 96].y, BLUE, 5);
        gui_draw_line_width (120, 120, clock_pos_min[(minutes+60-15) % 60].x,     clock_pos_min[(minutes+60-15) % 60].y, GREEN, 3);
        gui_draw_line       (120, 120, clock_pos_sec[(seconds+60-15) % 60].x,     clock_pos_sec[(seconds+60-15) % 60].y, RED);
        if (count++ == 10) {
            if (seconds++ == 59) {
                if (minutes++ == 59) {
                    if (hours++ == 23) {
                        hours = 0;
                    }
                    minutes = 0;
                }
                seconds=0;
            }
            count = 0;
        }

        LCD_1IN28_Display(screen);
        DEV_Delay_ms(100);
    }

    DEV_Module_Exit();
}
