#include "pico/stdlib.h"

/*
 * We are blinking all the LEDs in a row (and not just one)
 * to be sure, that any LED attached to any LED will blink for sure.
 */
const uint LED_PINS[]= {  5,  4,  3,  2,  1,  0, 13, 14, 15, /* skip SWCLK, SWDIO */ 
                         16, 17, 18, 19, 20, 21, 22, /*skip ADC*/ 28, 27, 26 };

int main() {
    const uint num_pins = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

    for (int i = 0; i < num_pins; i++) {
        gpio_init(LED_PINS[i]);
        gpio_set_dir(LED_PINS[i], GPIO_OUT);
    }
    while (true) {
        for (int i = 0; i < num_pins; i++)
            gpio_put(LED_PINS[i], 1);
        sleep_ms(500);
        for (int i = 0; i < num_pins; i++)
            gpio_put(LED_PINS[i], 0);
        sleep_ms(500);
    }
}
