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
/*
 #include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define GPIO_BUTTON    0
#define GPIO_LED_RED   3
#define GPIO_LED_GREEN 4
#define GPIO_LED_BLUE  5

int button_press = 0;
int last_button_press = 0;

/** TODO: You will need more data to communicate from Interrupt handler gpio_callback */
/*
void gpio_callback(uint gpio, uint32_t events)
{
    /** TODO: Program filter routine for the evenst */
/*
}

void main(void)
{
    stdio_init_all();

    /** TODO: Fill in Initialization */

    // Keep the busy loop outputting
    /*
    while (true)
    {
        if (last_button_press != button_press)
        {
            printf("Number of times button pressed:%d\n", button_press);
            last_button_press = button_press;
        }
        sleep_ms (20);
    }
}
*/

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
#include "hardware/timer.h" // Nötig für die Zeitfunktionen

#define GPIO_BUTTON    0
#define GPIO_LED_RED   3
#define GPIO_LED_GREEN 4
#define GPIO_LED_BLUE  5

// Variablen, die im Interrupt (ISR) und in der main() genutzt werden, müssen 'volatile' sein!
volatile int button_press = 0;
int last_button_press = 0;

// Variable zum Speichern des Zeitpunkts, an dem der Taster gedrückt wurde
volatile absolute_time_t press_time;

/** TODO: You will need more data to communicate from Interrupt handler gpio_callback */

void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == GPIO_BUTTON) {
        if (events & GPIO_IRQ_EDGE_RISE) {
            // TASTER WIRD GEDRÜCKT (Steigende Flanke)
            // 1. Zeitstempel speichern
            press_time = get_absolute_time();
            
            // 2. Alle LEDs ausschalten
            gpio_put(GPIO_LED_RED, 0);
            gpio_put(GPIO_LED_GREEN, 0);
            gpio_put(GPIO_LED_BLUE, 0);

        } else if (events & GPIO_IRQ_EDGE_FALL) {
            // TASTER WIRD LOSGELASSEN (Fallende Flanke)
            // 1. Vergangene Zeit in Millisekunden berechnen
            int64_t duration_us = absolute_time_diff_us(press_time, get_absolute_time());
            int64_t duration_ms = duration_us / 1000;

            // 2. Filter-Logik anwenden
            if (duration_ms < 200) {
                // Prellen / Zu kurz -> ROT
                gpio_put(GPIO_LED_RED, 1);
            } else if (duration_ms <= 1000) {
                // Normaler Tastendruck -> GRÜN & Zähler erhöhen
                gpio_put(GPIO_LED_GREEN, 1);
                button_press++;
            } else {
                // Langer Tastendruck -> BLAU & Zähler erhöhen
                gpio_put(GPIO_LED_BLUE, 1);
                button_press++;
            }
        }
    }
}

int main(void)
{
    stdio_init_all();

    /** TODO: Fill in Initialization */
    // 1. LEDs initialisieren und auf OUTPUT setzen
    gpio_init(GPIO_LED_RED);
    gpio_set_dir(GPIO_LED_RED, GPIO_OUT);
    
    gpio_init(GPIO_LED_GREEN);
    gpio_set_dir(GPIO_LED_GREEN, GPIO_OUT);
    
    gpio_init(GPIO_LED_BLUE);
    gpio_set_dir(GPIO_LED_BLUE, GPIO_OUT);

    // 2. Taster initialisieren, auf INPUT setzen und Pull-Down aktivieren
    gpio_init(GPIO_BUTTON);
    gpio_set_dir(GPIO_BUTTON, GPIO_IN);

    // 3. Interrupt für den Taster registrieren (Löst bei steigender UND fallender Flanke aus)
    gpio_set_irq_enabled_with_callback(GPIO_BUTTON, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Keep the busy loop outputting
    while (true)
    {
        if (last_button_press != button_press)
        {
            printf("Number of times button pressed:%d\n", button_press);
            last_button_press = button_press;
        }
        sleep_ms(20);
    }
    
    return 0;
}