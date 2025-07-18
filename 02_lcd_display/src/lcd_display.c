#include "pico/stdlib.h"
#include "lib/lcd_display"

const char black[3] = {0x00, 0x00, 0x00};
const char white[3] = {0xff, 0xff, 0xff};

int main() {
    lcd_display_init(void);
    while (true) {
        lcd_display_bg(black);
        sleep_ms(500);
        lcd_display_bg(white);
        sleep_ms(500);
    }
}