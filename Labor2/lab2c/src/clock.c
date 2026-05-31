/**
 * @file Implements the main loop initializing components
 */
#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "hardware/ticks.h"
#include "hardware/gpio.h"

#include "clock.h"
#include "clock_cursor.h"
#include "clock_gui.h"
#include "clock_time.h"

#define GPIO_TZ_BUTTON 13

int main()
{
    static uint64_t tick_us;

    if (DEV_Module_Init() != 0)
        ERROR("DEV_Module_Init", -1);

    clock_gui_init();
    clock_gui_update(0);

    box_t box_hour =   { {70, 180}, {90, 200} };
    box_t box_minute = { {90, 180}, {110, 200} };
    clock_cursor_init(box_hour, box_minute);

    // Initialize the timezone button, set it as input and pull-up
    gpio_init(GPIO_TZ_BUTTON);
    gpio_set_dir(GPIO_TZ_BUTTON, GPIO_IN);
    gpio_pull_up(GPIO_TZ_BUTTON);

    // Initialize variables connected to the timezone button
    int current_tz = TIMEZONE_UTC;
    bool tz_btn_was_pressed = false;
    uint64_t last_tz_btn_time = 0;

    while (true) {
        bool update_gui = false;

        tick_us = time_us_64();
        
        /** @TODO: You may need to add/change logic */    

        // Check the cursor for movement and whether it's supposed to hide (after a few seconds)
        if (clock_cursor_update(tick_us))
            update_gui = true;

        clock_cursor_edit_t edit = clock_cursor_get_state(NULL, NULL);

        // Only update the seconds, if we're not updating the hours / minutes with the cursor
        if (edit == CURSOR_EDIT_NONE && clock_time_inc_second(tick_us))
            update_gui = true;
        
        // If the time is edited (hour or minute) we need to update the GUI
        if (edit != CURSOR_EDIT_NONE)
            update_gui = true;

        bool tz_btn_pressed = (gpio_get(GPIO_TZ_BUTTON) == 0);
        // only change timezone of the button once in case of a press (even if the button is spam pressed a lot in a short interval)
        if (tz_btn_pressed) {
            if (!tz_btn_was_pressed) {
                if (tick_us - last_tz_btn_time > 200000) {
                    last_tz_btn_time = tick_us;
                    tz_btn_was_pressed = true;

                    // toggle edit mode
                    if (edit == CURSOR_EDIT_NONE) {
                        // change timezone through incrementing in range 1-4 (number of tz)
                        current_tz = (current_tz + 1) % TIMEZONE_COUNT;
                        clock_time_set_timezone((timezones_t)current_tz);
                        update_gui = true;
                    }
                }
            }
        }
        else {
            tz_btn_was_pressed = false;
        }
        
        if (update_gui)
            clock_gui_update(tick_us);
    }

    // This is not really necessary, will never reach, dead-code-elimination
    DEV_Module_Exit();
}