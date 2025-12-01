#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "lcd/LCD_1in28.h"

/* We need to reverse the 2-byte color codes */
#define CONV_RGB(c) ((c<<8) & 0xff00 | (c>>8))
// #define CONV_RGB(c) (c)

#define ERROR(str,err) do {                    \
        fprintf(stderr, "ERROR (%s:%d): %s\n", \
                __FILE__, __LINE__, (str));    \
        exit(err);                             \
    } while(0)

/**
 * Function img_bars_create fills the image with 4 equal-sized columns
 * The function fills the image with columns colored RED, GREEN, BLUE
 * and WHITE (in their converted fashion)
 * 
 * @param x1    The start position in X direction (must be >= 0 and < 240)
 * @param y1    The start position in Y direction (must be >= 0 and < 240)
 * @param x2    The end position in X direction (must be >= 0 and < 240)
 * @param y2    The end position in Y direction (must be >= 0 and < 240)
 * @param image The image buffer to fill
 */
static void img_bars_create(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t * image) {
    assert (x1 >= 0 && x1 < 240);
    assert (y1 >= 0 && y1 < 240);
    assert (x2 >= 0 && x2 < 240);
    assert (y2 >= 0 && y2 < 240);
    assert (x2 > x1);
    assert (y2 > y1);
    int xdiff = (x2 - x1) / 4;
    for (int i = x1; i < x2; i++) {
        for (int j = y1; j < y2; j++) {
            if (i < (x1 + 1 * xdiff))
                image[i + j*LCD_1IN28_WIDTH] = CONV_RGB(RED);
            else if (i < (x1 + 2 * xdiff))
                image[i + j*LCD_1IN28_WIDTH] = CONV_RGB(GREEN);
            else if (i < (x1 + 3 * xdiff))
                image[i + j*LCD_1IN28_WIDTH] = CONV_RGB(BLUE);
            else
                image[i + j*LCD_1IN28_WIDTH] = CONV_RGB(WHITE);
        }
    }
}

int main() {
    uint16_t * image;
    uint16_t color = 0;
    if (DEV_Module_Init() != 0)
        ERROR("DEV_Module_Init", -1);

    /* LCD Init */
    printf("1.28inch LCD demo of images...\r\n");
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(MAGENTA);
    DEV_SET_PWM(60); // Required to get the Display to work

    /* Creating a full-sized image -- each Pixel has 2 Bytes*/
    image = malloc (LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT * 2);
    if (NULL == image)
        ERROR("malloc", -1);

    /* Fill it with the color white (no conversion needed) */
    for (int i = 0; i < LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT; i++) {
        image[i] = WHITE;
    }

    printf("Displaying a sub-window\n");
    uint16_t xstart = 50;
    uint16_t ystart = 70;
    uint16_t len = 64;
    img_bars_create(xstart, ystart, xstart + len, ystart + len, image);
    LCD_1IN28_DisplayWindows(xstart, ystart, xstart + len, ystart + len, image);

    /* Additionally show how the duty-cycle of the background-lighting works */
    int pwm_duty = 60;
    int pwm_inc = 0;
    while (true) {
        DEV_SET_PWM(pwm_duty);

        if (pwm_inc) {
            if (pwm_duty < 100)
                pwm_duty += 5;
            else
                pwm_inc = 0;
        } else {
            if (pwm_duty > 0)
                pwm_duty -= 5;
            else
                pwm_inc = 1;
        }
        printf("pwm_duty:%d\n", pwm_duty);
        sleep_ms(100);
    }

    DEV_Module_Exit();
}
