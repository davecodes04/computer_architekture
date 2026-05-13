/**
 * @file Implements the main loop initializing components
 */
#include "pico/stdlib.h"
#include "Config/DEV_Config.h"
#include "hardware/ticks.h"

#include "clock.h"
#include "clock_cursor.h"
#include "clock_gui.h"
#include "clock_time.h"

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

    while (true) {
        bool update_gui = false;

        tick_us = time_us_64();

        
        /** @TODO: You may need to add/change logic */    

        // Check the cursor for movement / and whether it's supposed to hide (after a few seconds)
        if (clock_cursor_update(tick_us))
            update_gui = true;

        clock_cursor_edit_t edit = clock_cursor_get_state(NULL, NULL);

        // Only update the seconds, if we're not updating the hours / minutes with the cursor
        if (edit == CURSOR_EDIT_NONE && clock_time_inc_second(tick_us))
            update_gui = true;
        
        // If the time is edited (hour or minute) we need to update the GUI
        if (edit != CURSOR_EDIT_NONE)
            update_gui = true;

    
        
        if (update_gui)
            clock_gui_update(tick_us);
    }

    // This is not really necessary, will never reach, dead-code-elimination
    DEV_Module_Exit();
}
