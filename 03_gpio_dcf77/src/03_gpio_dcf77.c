#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>

#define GPIO_LED 5

/*
 * Do we want to measure voltage levels (raw value 0...4095) using ADC or
 * do we use a GPIO to get the input value (low or high).
 */
// #define WANT_ADC

#ifdef WANT_ADC
#define DCF77_IO 28
#else
#define DCF77_IO 13
#endif

static struct {
  bool is_pulled_up;
  bool is_pulled_down;
  bool is_input_hysteresis_enabled;
  enum gpio_drive_strength drive_strength;
  enum gpio_slew_rate slew_rate;
} state;

int main() {
  stdio_init_all();

  gpio_init(GPIO_LED);
  gpio_set_dir(GPIO_LED, true); // Set as output

#ifdef WANT_ADC
  adc_init();
  adc_gpio_init(DCF77_IO);
#else
  gpio_init(DCF77_IO);
  gpio_set_dir(DCF77_IO, GPIO_IN);
  gpio_set_function(DCF77_IO, GPIO_FUNC_SIO);
  // gpio_set_input_hysteresis_enabled(DCF77_IO, false);
  // gpio_set_slew_rate(DCF77_IO, GPIO_SLEW_RATE_SLOW);
  // gpio_set_drive_strength(DCF77_IO, GPIO_DRIVE_STRENGTH_2MA);
  // gpio_set_slew_rate(DCF77_IO, GPIO_SLEW_RATE_FAST);
  // gpio_set_input_hysteresis_enabled(DCF77_IO, false);
  // gpio_pull_up(DCF77_IO);
  // gpio_pull_down(DCF77_IO);
  gpio_disable_pulls(DCF77_IO);
  // gpio_put(DCF77_IO, false);
  state.is_pulled_up = gpio_is_pulled_up(DCF77_IO);
  state.is_pulled_down = gpio_is_pulled_down(DCF77_IO);
  state.is_input_hysteresis_enabled =
      gpio_is_input_hysteresis_enabled(DCF77_IO);
  state.drive_strength = gpio_get_drive_strength(DCF77_IO);
  state.slew_rate = gpio_get_slew_rate(DCF77_IO);
#endif

  while (true) {
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / ((1 << 12) - 1);
    bool dcf77;

#ifdef WANT_ADC
    adc_select_input(2);
    uint16_t dcf77_raw = adc_read();
    printf("Raw ADC value: dcf77:%d (%f V)\n", dcf77_raw,
           dcf77_raw * conversion_factor);
    dcf77 = false;
    if (dcf77_raw > 4000)
      dcf77 = true;
#else
    dcf77 = gpio_get(DCF77_IO);
    printf("DCF77 value:%d pulled_up:%s pulled_down:%s input_hysteresis:%s "
           "drive_strength:%d slew_rate:%d\n",
           dcf77 ? 1 : 0, state.is_pulled_up ? "yes" : "no",
           state.is_pulled_down ? "yes" : "no",
           state.is_input_hysteresis_enabled ? "yes" : "no",
           state.drive_strength, state.slew_rate);
#endif

    gpio_put(GPIO_LED, dcf77);

    sleep_ms(50);
  }
}
