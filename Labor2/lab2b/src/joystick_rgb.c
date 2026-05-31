/**
 * \file Make the Joystick steer the RGB-LED.
 * The Joystick has two potentionmeter for the X- and the Y-axis.
 * These may be read out using the GPIO pins that support converting
 * voltages into digital values using the ADC component with it's
 * 12-bit quantization using successive-approximation ADC (SAR).
 *
 * Read out the X-axis to color-code the RGB LED in a loop.
 * 
 * With Pulse-Width-Modulation (PWM) one may define a duty-cycle
 * from 0 to 100% (0 to 254) on a GPIO initialized to output using
 * a digital signal with PWM. Here the actual timing is not relevant
 * (in comparison to servo-motors and alike).
 * You shall light the PWM from RED to BLUE: 100% RED when the X-axis
 * is at the maximum deflection, and 100% BLUE when the X-axis is at 
 * the lowest/minimum deflection.
 * Instead of any color-correction / brightness-correction, just linearly
 * scale between those two value, as in BLUE=100-RED.
 */
#include <stdio.h>
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

// defining X and Y pins for joystick and the LED pins
#define GPIO_X         26
#define GPIO_Y         27
#define GPIO_LED_RED    5
#define GPIO_LED_GREEN  3
#define GPIO_LED_BLUE   4

int main() {
    /** TODO: Initialize ADC to read out  */
    adc_init();
    adc_gpio_init(GPIO_X);

    /** TODO: Initialize PWM for the LEDs  */
    gpio_set_function(GPIO_LED_RED, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_LED_BLUE, GPIO_FUNC_PWM);

    // find out which slice number (pwm channel number) is connected to the pins
    uint slice_number_red = pwm_gpio_to_slice_num(GPIO_LED_RED);
    uint slice_number_blue = pwm_gpio_to_slice_num(GPIO_LED_BLUE);

    // set the wrap value (max counter value for PWM)
    pwm_set_wrap(slice_number_red, 254);
    pwm_set_wrap(slice_number_blue, 254);

    // set the pwm running
    pwm_set_enabled(slice_number_red, true);
    pwm_set_enabled(slice_number_blue, true);
    

    while (true)
    {
        uint16_t x;
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / ((1 << 12) - 1);
        
        /** TODO: Read out the joysticks X-Value */
        adc_select_input(0); // GPIO_X is ADC input 0 - according to documentation (channel A)
        x = adc_read();

        /** TODO: Change the PWM-Level from 100% RED at maximum X - Level to 100% BLUE at minium  */
        // scale the red value to the range of 0 to 254
        uint16_t  red_pwm = (x * 254) / 4095;
        // flip the red to blue
        uint16_t blue_pwm = 254 - red_pwm;

        // controll the leds according to calculated pwm values
        pwm_set_gpio_level(GPIO_LED_RED, red_pwm);
        pwm_set_gpio_level(GPIO_LED_BLUE, blue_pwm);

        // prints the raw joystick value
        printf("Raw joystick value: x:%4d (%1.3f V)\n",
               x, x * conversion_factor);
        sleep_ms(50);
    }
}