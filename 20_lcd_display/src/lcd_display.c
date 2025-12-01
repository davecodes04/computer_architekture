#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "lcd/LCD_1in28.h"

const int colors[] = {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, WHITE};
const int num_colors = (sizeof(colors)/sizeof(colors[0]));

int main() {
    uint16_t color = 0;
    if (DEV_Module_Init() != 0)
        return -1;

    /* LCD Init */
    printf("1.28inch LCD demo...\r\n");
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(WHITE);
    DEV_SET_PWM(60); // Turn on LCD Background lighting with 60%
    while (true) {
        LCD_1IN28_Clear(colors[color]);
        color = (color+1) % num_colors;
        sleep_ms(500);
        printf("color:%d\n", color);
    }
    DEV_Module_Exit();
}
