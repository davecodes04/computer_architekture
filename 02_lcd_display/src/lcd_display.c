#include "pico/stdlib.h"
#include "lcd/lcd.h"

const char black[3] = {0x00, 0x00, 0x00};
const char white[3] = {0xff, 0xff, 0xff};

int main() {
    lcd_init();
    while (true) {
        lcd_circle(LCD_W2, LCD_H2, LCD_W/2, 0x00, 1, true);
        sleep_ms(500);
        lcd_circle(LCD_W2, LCD_H2, LCD_W/2, 0xffff, 1, true);
        sleep_ms(500);
    }
}