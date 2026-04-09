#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define GPIO_X      26
#define GPIO_Y      27
#define GPIO_BUTTON 16

/* BEGIN: JUST FOR DEBUGGING */
struct state {
    bool is_pulled_up;
    bool is_pulled_down;
    bool is_input_hysteresis_enabled;
    enum gpio_drive_strength drive_strength;
    enum gpio_slew_rate slew_rate;
};

void gpio_get_state(uint gpio, struct state * gpio_state) {
    assert (NULL != gpio_state);
    gpio_state->is_pulled_up = gpio_is_pulled_up(gpio);
    gpio_state->is_pulled_down = gpio_is_pulled_down(gpio);
    gpio_state->is_input_hysteresis_enabled = gpio_is_input_hysteresis_enabled (gpio);
    gpio_state->drive_strength = gpio_get_drive_strength (gpio);
    gpio_state->slew_rate = gpio_get_slew_rate (gpio);
}
/* END: JUST FOR DEBUGGING */

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(GPIO_X);
    adc_gpio_init(GPIO_Y);

    gpio_init(GPIO_BUTTON);           // The GPIO connecting button of this joystick
    gpio_set_dir(GPIO_BUTTON, false); // Set as input
    gpio_pull_up(GPIO_BUTTON);        // Enable Pull-UP Resistor (inverts the button)

    struct state input_state;
    gpio_get_state (GPIO_BUTTON, &input_state);

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
