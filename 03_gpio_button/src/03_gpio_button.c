#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define GPIO_BUTTON 18

// #define WANT_PULL_DOWN_EXTERNAL
// #define WANT_PULL_DOWN
#define WANT_PULL_UP

#if ((defined(WANT_PULL_DOWN_EXTERNAL) && (defined(WANT_PULL_DOWN) || defined(WANT_PULL_UP))) || \
     (defined(WANT_PULL_DOWN) && (defined(WANT_PULL_DOWN_EXTERNAL) || defined(WANT_PULL_UP))) || \
     (defined(WANT_PULL_UP) && (defined(WANT_PULL_DOWN_EXTERNAL) || defined(WANT_PULL_DOWN))))
#   error "ERROR: Should only define one of three pull-up/pull-down variants"
#endif

// Uncomment and define, if You want debugging of GPIO state
#define WANT_DEBUG

/*************** Function definitions ***********************************************/
static void gpio_event_string(char *buf, uint32_t events);
static void my_gpio_callback(uint gpio, uint32_t events);

#ifdef WANT_DEBUG
struct gpio_state;    // Forward declaration
static void get_gpio_state(uint gpio, struct gpio_state * gpio_state);
#endif /* WANT_DEBUG */
/************************************************************************************/

// See rp2350-datasheet.pdf (p. 604)
static const char * gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};
static char event_str[128];

#ifdef WANT_DEBUG
struct gpio_state {
    bool is_pulled_up;
    bool is_pulled_down;
    bool is_input_hysteresis_enabled;
    enum gpio_drive_strength drive_strength;
    enum gpio_slew_rate slew_rate;
};

static void get_gpio_state(uint gpio, struct gpio_state * gpio_state) {
    assert (NULL != gpio_state);
    gpio_state->is_pulled_up = gpio_is_pulled_up(gpio);
    gpio_state->is_pulled_down = gpio_is_pulled_down(gpio);
    gpio_state->is_input_hysteresis_enabled = gpio_is_input_hysteresis_enabled (gpio);
    gpio_state->drive_strength = gpio_get_drive_strength (gpio);
    gpio_state->slew_rate = gpio_get_slew_rate (gpio);
}
#endif /* WANT_DEBUG */

int main() {
    stdio_init_all();

    gpio_init(GPIO_BUTTON);                     // The GPIO connecting button
    gpio_set_dir(GPIO_BUTTON, false);           // Set as input

#if defined(WANT_PULL_DOWN_EXTERNAL)
    gpio_set_pulls(GPIO_BUTTON, false, false);
#elif defined(WANT_PULL_DOWN)
    gpio_pull_down(GPIO_BUTTON);
#elif defined(WANT_PULL_UP)
    gpio_pull_up(GPIO_BUTTON);
#endif

#ifdef WANT_DEBUG
    struct gpio_state input_state;
    get_gpio_state (GPIO_BUTTON, &input_state);
#endif /* WANT_DEBUG */

    gpio_set_irq_enabled_with_callback(GPIO_BUTTON, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &my_gpio_callback);

    // Busy-loop forever
    while (true);
}


static void my_gpio_callback(uint gpio, uint32_t events) {
    gpio_event_string(event_str, events);
    static int button_press = 0;
#if defined(WANT_PULL_DOWN_EXTERNAL) || defined(WANT_PULL_DOWN)
    if (/* EVENT_RISE */ 0x8 == events) 
        button_press++;
#else
    if (/* EVENT_FALL */ 0x4 == events) 
        button_press++;
#endif
    printf("GPIO %d %s number pressed:%d\n", gpio, event_str, button_press);
}


/**
 * Translates the up to 4 GPIO IRQ Events into the string buf.
 *
 * Parses the 4 possible events using a Mask and "translates" those into
 * human-readable String -- which needs to be large enough to possible 48
 * characters (plus the NUL byte).
 * See rp2350-datasheet.pdf (p. 604)
 *
 * @param[out] buf    Pointer to a string large enough to hold 4 event names (plus ", " and NUL)
 * @param[in]  events The events provided to the Callback handler
 */
static void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}
