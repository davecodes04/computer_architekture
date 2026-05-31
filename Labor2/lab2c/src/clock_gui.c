/**
 * @file Implements the main GUI of this clock.
 * 
 * Based on simple lib/gui, which is based on lib/lcd
 * Major performance uptake is the introduction of a
 * backup image to restore, aka draw once, restore and
 * draw on-top.
 */
#include "lcd/LCD_1in28.h"
#include "gui/gui.h"
#include "math.h"
#include "clock.h"
#include "clock_cursor.h"
#include "clock_gui.h"
#include "clock_time.h"

/*
 * Include the header with an image created from Gimp:
 * A 240x240 pixel image with blue as "green-screen".
 *
 * We *could* export as .c-File even with RGB565 data,
 * But that would not help much, wed need to copy it
 * byte-for-byte, as well.
 */
#include "../images/watch_man.h"

// selects if we want the 12 hour (1) or the 24 hour mode (0)
#define SELECT12HOURS 1
#define CURSOR_LEN  5

// Main memory for drawing screen, registered to "gui"
static uint16_t *screen;

// 60 end positions on the outskirts of the display
static pos_t clock_pos_sec[60];
static pos_t clock_pos_min[60];
static pos_t clock_pos_hrs[96];

static void calc_clock_pos(void)
{
    for (int r = 0; r < 60; r++)
    {
        int x = 120 + round(110.0 * cos((360.0 / 60.0 * r) * M_PI / 180.0));
        int y = 120 + round(110.0 * sin((360.0 / 60.0 * r) * M_PI / 180.0));
        clock_pos_sec[r].x = x;
        clock_pos_sec[r].y = y;
    }

    for (int r = 0; r < 60; r++)
    {
        int x = 120 + round(90.0 * cos((360.0 / 60.0 * r) * M_PI / 180.0));
        int y = 120 + round(90.0 * sin((360.0 / 60.0 * r) * M_PI / 180.0));
        clock_pos_min[r].x = x;
        clock_pos_min[r].y = y;
    }

    // Even for hours, we split into 96 separate positions
    for (int r = 0; r < 96; r++)
    {
        int x = 120 + round(70.0 * cos((360.0 / 96.0 * r) * M_PI / 180.0));
        int y = 120 + round(70.0 * sin((360.0 / 96.0 * r) * M_PI / 180.0));
        clock_pos_hrs[r].x = x;
        clock_pos_hrs[r].y = y;
    }
}

void clock_gui_init(void)
{
    LCD_1IN28_Init(HORIZONTAL);

    /* Creating a full-sized image -- each Pixel has 2 Bytes*/
    screen = malloc(LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT * 2);
    if (NULL == screen)
        ERROR("malloc", -1);

    /* Fill it with the color black (no conversion needed) */
    for (int i = 0; i < LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT; i++)
    {
        screen[i] = BLACK;
    }

    gui_image_new((uint8_t *)screen, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, ROTATE_0, BLACK);
    // Nice grey background
    for (int r = -1; r < 10; r++)
    {
        int grey = 10 * abs(r) + 1;
        gui_draw_circle(120, 120, 120 - r, GUI_RGB(grey, grey, grey), DRAW_FILL_EMPTY);
    }
    // Draw 12x the hour line
    for (int r = 0; r < 12; r++)
    {
        int x1 = 120 + round(108.0 * sin(30.0 * r * M_PI / 180.0));
        int y1 = 120 + round(108.0 * cos(30.0 * r * M_PI / 180.0));
        int x2 = 120 + round(120.0 * sin(30.0 * r * M_PI / 180.0));
        int y2 = 120 + round(120.0 * cos(30.0 * r * M_PI / 180.0));

        gui_draw_line(x1, y1, x2, y2, WHITE);
    }

    // Extract pixels from bitmap-image (8-bit RGB values)
    char *data = header_data;
    char pixel[3];
    for (int i = 0; i < width * height; i++)
    {
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

    calc_clock_pos();

    LCD_1IN28_Display(screen);
}

void clock_gui_update(uint64_t tick_us)
{
    int hours_arr;
    char time[16];
    int time_len;

    int hours, minutes, seconds;

    clock_time_get_local(&hours, &minutes, &seconds);

    gui_image_restore();

    // First draw the cursor
    pos_t pos;
    bool visible;
    clock_cursor_edit_t edit = clock_cursor_get_state(&pos, &visible);

    if (visible) {
        int start, end;
        start = MAX(0, pos.x - CURSOR_LEN);
        end = MIN(LCD_1IN28_WIDTH, pos.x + CURSOR_LEN);
        gui_draw_line(start, pos.y, end, pos.y, RED);
        start = MAX(0, pos.y - CURSOR_LEN);
        end = MIN(LCD_1IN28_HEIGHT, pos.y + CURSOR_LEN);
        gui_draw_line(pos.x, start, pos.x, end, RED);
    }

    // The color of the to-be-edited hour / minute
    uint16_t color;

// change to 12 hour mode if selected
#if SELECT12HOURS == 1
    // we technically still use the 24 hour format, but then % 12 it to get the correct values for this mode
    int display_hours = hours % 12;
    const char *am_pm;

    // display 12 if it is currently 0
    if (display_hours == 0) {
        display_hours = 12;
    }

    // set AM or PM
    if (hours < 12) {
        am_pm = "AM";
    }
    else {
        am_pm = "PM";
    }
    
    // display the hours
    color = (edit == CURSOR_EDIT_HOUR) ? BLUE : GREEN;
    snprintf(time, sizeof(time), "%0d:", display_hours);
    gui_draw_string(60, 180, time, &Font24, color, WHITE);

    // display the minutes
    color = (edit == CURSOR_EDIT_MINUTE) ? BLUE : GREEN;
    snprintf(time, sizeof(time), "%0d", minutes);
    gui_draw_string(100, 180, time, &Font24, color, WHITE);

    // display AM/PM indicator
    gui_draw_string(140, 180, am_pm, &Font24, color, WHITE);

// change to 24 hour mode if selected
#else 
    color = (edit == CURSOR_EDIT_HOUR) ? BLUE : GREEN;
    snprintf(time, sizeof(time), "%0d:", hours);
    gui_draw_string(80, 180, time, &Font24, color, WHITE);

    color = (edit == CURSOR_EDIT_MINUTE) ? BLUE : GREEN;
    snprintf(time, sizeof(time), "%0d", minutes);
    gui_draw_string(120, 180, time, &Font24, color, WHITE);
#endif

    // Now draw the clock's hands
    hours_arr = hours * 8 + (minutes * 16) / 120;
    gui_draw_line_width(120, 120, clock_pos_hrs[(hours_arr + 96 - 24) % 96].x, clock_pos_hrs[(hours_arr + 96 - 24) % 96].y, GREEN, 5);
    gui_draw_line_width(120, 120, clock_pos_min[(minutes + 60 - 15) % 60].x, clock_pos_min[(minutes + 60 - 15) % 60].y, GREEN, 3);
    gui_draw_line(120, 120, clock_pos_sec[(seconds + 60 - 15) % 60].x, clock_pos_sec[(seconds + 60 - 15) % 60].y, RED);

    LCD_1IN28_Display(screen);
}