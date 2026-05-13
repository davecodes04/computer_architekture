/**
 * @file Implements the cursor allowing to set the time:
 *  - when moving the Joystick, a cursor (two lines) should appear
 *    (see clock_gui.c)
 *  Using the joystack, one shall edit (change) the hour and minute:
 *  - when not in editing mode:
 *    - Pulling the joystick position of the cursor should change:
 *       pull up: decrease y  /  pull down: increase y
 *       pull left: decrease x / pull right: increase x
 *    - when clicking on hour / minute: Select Edit mode for hour or minute
 *  - when in editing mode:
 *    - Pulling the joystick should change the hour / minute:
 *       pull up: increase hour or minute / pull down: decrease hour or minute
 *    - When clicking: Get out of Editing mode
 */
#include <stdio.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

#include "clock.h"
#include "clock_time.h"
#include "clock_cursor.h"

#define CURSOR_VISIBLE_TIME (5 * 1000 * 1000) // 5 seconds
#define CURSOR_BUTTON_PRESS_TIME (200 * 1000) // 200 ms

/*
 * The cursors current state (position, mode of edit, visibility and
 * when that cursor was last visible.
 */
static pos_t cursor_pos = {120, 120};
/** @TODO: You may need to add more local data */


void clock_cursor_init(box_t box_hour, box_t box_minute)
{
    /** @TODO: You need to implement the initialization */
}

/*
 * Confert adc readout to a sensical pointer change value;
 */
static int16_t adc_to_pos_value(uint16_t adc)
{
    /** @TODO: You need to implement logic */
    return 0;
}

/*
 * Confert adc readout to a sensical pointer change value;
 */
static int16_t adc_to_timechange_value(uint16_t adc)
{
    /** @TODO: You need to implement logic */
    return 0;
}


bool clock_cursor_update(uint64_t tick_us)
{
    /** @TODO: You need to implement logic */
    return false;
}

clock_cursor_edit_t clock_cursor_get_state(pos_t *pos, bool *visible)
{
    if (NULL != pos)
    {
        pos->x = cursor_pos.x;
        pos->y = cursor_pos.y;
    }
    if (NULL != visible)
    {
        *visible = cursor_visible;
    }
    return cursor_edit;
}
