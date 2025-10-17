#include "pico/stdlib.h"
#include "pico/rand.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

const unsigned int RGB_LED_PINS[] = {1, 3, 4};

static unsigned int iobank_slice[3];
static unsigned int iobank_num = 1;
static pwm_config config;

typedef enum
{
    RGB_RED_UP = 0,
    RGB_GREEN_UP,
    RGB_BLUE_UP,
    RGB_RED_DOWN,
    RGB_GREEN_DOWN,
    RGB_BLUE_DOWN
} rgb_state_t;

void on_pwm_wrap()
{
    int need_new_state = 0;
    static int red = 0;
    static int green = 0;
    static int blue = 0;
    static rgb_state_t state = RGB_BLUE_UP;
    // Clear the interrupt flag that brought us here
    // pwm_clear_irq(pwm_gpio_to_slice_num(RGB_LED_PINS[0]));
    for (int i = 0; i < iobank_num; i++)
        pwm_clear_irq(iobank_slice[i]);

    switch (state)
    {
    case RGB_RED_UP:
        if (red < 255)
            red++;
        if (red == 255)
            need_new_state = 1;
        pwm_set_gpio_level(RGB_LED_PINS[0], red * red);
        break;
    case RGB_GREEN_UP:
        if (green < 255)
            green++;
        if (green == 255)
            need_new_state = 1;
        pwm_set_gpio_level(RGB_LED_PINS[1], green * green);
        break;
    case RGB_BLUE_UP:
        if (blue < 255)
            blue++;
        if (blue == 255)
            need_new_state = 1;
        pwm_set_gpio_level(RGB_LED_PINS[2], blue * blue);
        break;
    case RGB_RED_DOWN:
        if (red > 0)
            red--;
        if (red == 0)
            need_new_state = 1;
        pwm_set_gpio_level(RGB_LED_PINS[0], red * red);
        break;
    case RGB_GREEN_DOWN:
        if (green > 0)
            green--;
        if (green == 0)
            need_new_state = 1;
        pwm_set_gpio_level(RGB_LED_PINS[1], green * green);
        break;
    case RGB_BLUE_DOWN:
        if (blue > 0)
            blue--;
        if (blue == 0)
            need_new_state = 1;
        pwm_set_gpio_level(RGB_LED_PINS[2], blue * blue);
        break;
    }

    if (need_new_state)
        state = get_rand_32() % (RGB_BLUE_DOWN + 1);
}

int main()
{
    // Figure out which slice we just connected to the LED pin
    // With RP2350B there's two at max -- but for future HW, the pins
    // might be distributed across 3, so account for that
    iobank_slice[0] = pwm_gpio_to_slice_num(RGB_LED_PINS[0]);
    iobank_slice[1] = pwm_gpio_to_slice_num(RGB_LED_PINS[1]);
    iobank_slice[2] = pwm_gpio_to_slice_num(RGB_LED_PINS[2]);
    if (iobank_slice[0] != iobank_slice[1])
        iobank_num++;
    if (iobank_slice[0] != iobank_slice[2] && iobank_slice[1] != iobank_slice[2])
        iobank_num++;

    gpio_set_function(RGB_LED_PINS[0], GPIO_FUNC_PWM);
    gpio_set_function(RGB_LED_PINS[1], GPIO_FUNC_PWM);
    gpio_set_function(RGB_LED_PINS[2], GPIO_FUNC_PWM);
    pwm_set_gpio_level(RGB_LED_PINS[0], 0);
    pwm_set_gpio_level(RGB_LED_PINS[1], 0);
    pwm_set_gpio_level(RGB_LED_PINS[2], 255*255);

    // Mask out our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    for (int i = 0; i < iobank_num; i++)
    {
        pwm_clear_irq(iobank_slice[i]);
        pwm_set_irq_enabled(iobank_slice[i], 1);
    }
    irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_pwm_wrap);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), 1);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    for (int i = 0; i < iobank_num; i++)
        pwm_init(iobank_slice[i], &config, 1);

    // Everything after this point happens in the PWM interrupt handler, so we
    // can twiddle our thumbs
    while (1)
    {
    }
}