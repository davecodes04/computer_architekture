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

#define GPIO_X         26
#define GPIO_Y         27
#define GPIO_LED_RED    3
#define GPIO_LED_GREEN  4
#define GPIO_LED_BLUE   5

int main() {

    /** TODO: Initialize ADC to read out  */
    /** TODO: Initialize PWM for the LEDs  */
    
    while (true)
    {
        uint16_t x;
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / ((1 << 12) - 1);
        
        /** TODO: Read out the joysticks X-Value */
        /** TODO: Change the PWM-Level from 100% RED at maximum X - Level to 100% BLUE at minium  */

        printf("Raw joystick value: x:%4d (%1.3f V)\n",
               x, x * conversion_factor);
        sleep_ms(500);
    }
}
