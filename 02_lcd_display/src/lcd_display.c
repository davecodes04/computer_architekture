#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "lcd/LCD_1in28.h"

#define BLACK          0x0000
#define BLUE           0x001F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define RED            0xF800
#define MAGENTA        0xF81F
#define WHITE          0xFFFF
const int colors[] = {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, WHITE};

int main() {
    uint16_t i = 0;
    if (DEV_Module_Init() != 0)
        return -1;

    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);
    /* LCD Init */
    printf("1.28inch LCD demo...\r\n");
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(WHITE);
    DEV_SET_PWM(60);

    printf("Starting to rotate through colors\n");
    while (true) {
        LCD_1IN28_Clear(colors[i]);
        sleep_ms(250);

        i = (i+1) % (sizeof(colors)/sizeof(colors[0]));
        printf("%d\n", i);
    }

    /* Module Exit */
    DEV_Module_Exit();
}
