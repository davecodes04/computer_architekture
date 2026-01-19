#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define GPIO_X      26
#define GPIO_Y      27
#define GPIO_BUTTON 16

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(GPIO_X);
    adc_gpio_init(GPIO_Y);

    gpio_init(GPIO_BUTTON);           // The GPIO connecting button of this joystick
    gpio_set_dir(GPIO_BUTTON, false); // Set as input
    gpio_pull_up(GPIO_BUTTON);        // Enable Pull-UP Resistor (inverts the button)

    while (true)
    {
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / ((1 << 12) - 1);
        adc_select_input(0);
        uint16_t x = adc_read();
        adc_select_input(1);
        uint16_t y = adc_read();
        bool button = gpio_get(GPIO_BUTTON);

        printf("Raw joystick value: x:%4d (%1.3f V);\ty:%4d (%1.3f V)\tbutton %spressed\n",
               x, x * conversion_factor, y, y * conversion_factor,
               !button ? "" : "not ");

        sleep_ms(500);
    }
}
