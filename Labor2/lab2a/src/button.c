/**
 * @file Register a callback to the GPIO of a button, which You need to
 * initialized to either a external pull-down or an internal pull-up.
 *
 * This button is not debounced (no RC or RL filter in hardware).
 * Therefore You will need to debounce in software, using Pico's
 * low-level ticks interface or Pico's high-level time functions.
 *
 * The loop must count correctly the number of button presses.
 * Any button press > 200ms should be registered, any event shorter
 * than that filtered:
 * - Filtered button events should trigger light up only the RED LED
 * - Normal button presses should trigger light up only the GREEN LED
 * - Long button presses (>1s) should trigger light up only the BLUE LED
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "button.h"

// GPIO pins for the button and LED
#define GPIO_BUTTON 13
#define GPIO_LED_GREEN 3
#define GPIO_LED_BLUE 4
#define GPIO_LED_RED 5

// have to be volatile because they are used in the interrupt and main()
volatile int button_press = 0;
int last_button_press = 0;
volatile bool button_pressed = false;
volatile uint32_t press_time = 0;
volatile int32_t duration_ms;
volatile bool press_valid = false;

#define DEBOUNCE_TIME_MS 50

volatile absolute_time_t last_event_time;

/** TODO: You will need more data to communicate from Interrupt handler gpio_callback */
void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == GPIO_BUTTON)
    {
        uint32_t now = time_us_32();

        // where the debouncing takes place
        if (absolute_time_diff_us(last_event_time, now) < DEBOUNCE_TIME_MS * 1000) {
            return;
        }

        last_event_time = now;

        // Falling edge case (button pressed down)
        if (events == GPIO_IRQ_EDGE_FALL)
        {
            press_time = now;
            press_valid = true;
        }

        // Rising edge case (let go of button)
        if (events == GPIO_IRQ_EDGE_RISE)
        {
            if (!press_valid)
            {
                return;
            }
            press_valid = false;
            duration_ms = (now - press_time) / 1000;
            button_pressed = true;
        }
    }
}

void evaluate_led_state(int64_t duration_ms)
{
    // check duration and therefore which color to light up
    if (duration_ms < 200)
    {
        // disable other colors
        gpio_put(GPIO_LED_GREEN, 0);
        gpio_put(GPIO_LED_BLUE, 0);

        gpio_put(GPIO_LED_RED, 1); // green light is on
    }
    else if (duration_ms <= 1000)
    {
        // disable other colors
        gpio_put(GPIO_LED_RED, 0);
        gpio_put(GPIO_LED_BLUE, 0);

        gpio_put(GPIO_LED_GREEN, 1); // blue light is on
        button_press++; // button press only tracked for blue and red
    }
    else if (duration_ms > 1000)
    {
        // disable other colors
        gpio_put(GPIO_LED_RED, 0);
        gpio_put(GPIO_LED_GREEN, 0);
        
        gpio_put(GPIO_LED_BLUE, 1); // red light is on
        button_press++; // button press only tracked for blue and red
    }
}
int main(void)
{
    stdio_init_all();

    /** TODO: Fill in Initialization */
    // Initialize all LED's and set them to output
    gpio_init(GPIO_LED_RED);
    gpio_set_dir(GPIO_LED_RED, GPIO_OUT);
    gpio_init(GPIO_LED_GREEN);
    gpio_set_dir(GPIO_LED_GREEN, GPIO_OUT);
    gpio_init(GPIO_LED_BLUE);
    gpio_set_dir(GPIO_LED_BLUE, GPIO_OUT);

    // Initialize button and set input and to pull-up
    gpio_init(GPIO_BUTTON);
    gpio_set_dir(GPIO_BUTTON, GPIO_IN);
    gpio_pull_up(GPIO_BUTTON);

    // Interrupt called at either edge rise or edge fall
    gpio_set_irq_enabled_with_callback(GPIO_BUTTON, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (true)
    {
        // only if the button is truly pressed again, do we print out how many button presses there have been
        if (last_button_press != button_press)
        {
            printf("Number of times button pressed:%d\n", button_press);
            last_button_press = button_press;
        }
        // if the button is pressed once, it is then set to unpressed and the LED states are evaluated
        if (button_pressed)
        {
            button_pressed = false;
            evaluate_led_state(duration_ms);
        }
        sleep_ms(20);
    }
    return 0;
}