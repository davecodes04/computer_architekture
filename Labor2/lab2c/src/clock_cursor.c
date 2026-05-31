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

// variables for cursor state
static bool cursor_visible = true;
static clock_cursor_edit_t cursor_edit = CURSOR_EDIT_NONE;

// variables for cursor timing
static box_t b_hour;
static box_t b_minute;
static uint64_t last_visible_time = 0;
static uint64_t last_button_press_time = 0;
static bool button_was_pressed = false;

void clock_cursor_init(box_t box_hour, box_t box_minute)
{
    /** @TODO: You need to implement the initialization */
    b_hour = box_hour;
    b_minute = box_minute;

    // initialize ADC for joystick
    adc_init();
    adc_gpio_init(JOYSTICK_GPIO_X);
    adc_gpio_init(JOYSTICK_GPIO_Y);

    // initialize button for joystick
    gpio_init(JOYSTICK_GPIO_BUTTON);
    gpio_set_dir(JOYSTICK_GPIO_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_GPIO_BUTTON);
}

/*
 * Convert adc readout to a sensical pointer change value;
 */
static int16_t adc_to_pos_value(uint16_t adc)
{
    /** @TODO: You need to implement logic */

    // set movement speed for cursor position change

    // faster in one direction
    if (adc < 1000)
    {
        return -4;
    }
    // slower in the same direction
    if (adc < 1800)
    {
        return -2;
    }

    // faster in the other direction
    if (adc > 3100)
    {
        return 4;
    }
    // slower in the same direction
    if (adc > 2300)
    {
        return 2;
    }

    // joystick is in the middle, no movement
    return 0;
}

/*
 * Convert adc readout to a sensical pointer change value;
 */
static int16_t adc_to_timechange_value(uint16_t adc)
{
    /** @TODO: You need to implement logic */
    // set movement speed for time change when in edit mode
    if (adc < 1000)
    {
        return -2;
    }
    if (adc < 1800)
    {
        return -1;
    }
    if (adc > 3100)
    {
        return 2;
    }
    if (adc > 2300)
    {
        return 1;
    }
    return 0;
}

bool clock_cursor_update(uint64_t tick_us)
{
    /** @TODO: You need to implement logic */
    bool gui_needs_update = false;
    bool button_pressed = (gpio_get(JOYSTICK_GPIO_BUTTON) == 0);

    if (button_pressed)
    {
        if (!button_was_pressed)
        {
            // if button is pressed, check if it's a new press (debounce)
            if (tick_us - last_button_press_time > CURSOR_BUTTON_PRESS_TIME)
            {
                last_button_press_time = tick_us;
                button_was_pressed = true;

                last_visible_time = tick_us;
                cursor_visible = true;

                // toggle edit mode
                if (cursor_edit == CURSOR_EDIT_NONE)
                {
                    // check if cursor is in hour box
                    if (cursor_pos.x >= b_hour.upper_left.x && cursor_pos.x <= b_hour.lower_right.x &&
                        cursor_pos.y >= b_hour.upper_left.y && cursor_pos.y <= b_hour.lower_right.y)
                    {
                        cursor_edit = CURSOR_EDIT_HOUR;
                    }
                    // check if cursor is in minute box
                    else if (cursor_pos.x >= b_minute.upper_left.x && cursor_pos.x <= b_minute.lower_right.x &&
                             cursor_pos.y >= b_minute.upper_left.y && cursor_pos.y <= b_minute.lower_right.y)
                    {
                        cursor_edit = CURSOR_EDIT_MINUTE;
                    }
                }
                else
                {
                    // exit edit mode
                    cursor_edit = CURSOR_EDIT_NONE;
                }
                gui_needs_update = true;
            }
        }
    }
    else
    {
        button_was_pressed = false;
    }

    // read adc values for joystick
    adc_select_input(0); // X-axis
    uint16_t adc_x = adc_read();
    adc_select_input(1); // Y-axis
    uint16_t adc_y = adc_read();

    if (cursor_edit == CURSOR_EDIT_NONE)
    {
        // update cursor position based on joystick input
        int16_t pos_change_x = adc_to_pos_value(adc_x);
        int16_t pos_change_y = adc_to_pos_value(adc_y);

        if (pos_change_x != 0 || pos_change_y != 0)
        {
            cursor_pos.x += pos_change_x;
            cursor_pos.y += pos_change_y;

            // cursor should not go out of bounds (240x240 display)
            if (cursor_pos.x < 0) {
                cursor_pos.x = 0;
            }
            if (cursor_pos.x > 239) {
                cursor_pos.x = 239;
            }
            if (cursor_pos.y < 0) {
                cursor_pos.y = 0;
            }
            if (cursor_pos.y > 239) {
                cursor_pos.y = 239;
            }

            last_visible_time = tick_us;
            cursor_visible = true;
            gui_needs_update = true;
        }
    }
    // EDIT MODE: joystick changes time instead of cursor position
    else
    {
        static uint64_t last_edit_time = 0;
        // update time based on joystick input when in edit mode
        int16_t speed_flag = adc_to_timechange_value(adc_x);
        uint64_t delay_us = 0;

        // set delay between time changes according to joystick movement speed, so that time doesn't change too fast
        if (speed_flag != 0)
        {
            if (abs(speed_flag) == 2) {
                delay_us = 100000;
            }
            else {
                delay_us = 300000;
            }

            // check if delay has passed since last time change to avoid changing time too fast
            if ((tick_us - last_edit_time) > delay_us) {
                int8_t time_change = 0;
                
                // set value for time change according to joystick movement speed and direction
                if (speed_flag > 0) {
                    time_change = 1;
                }
                else {
                    time_change = -1;
                }
                // change either hour or minute based on the edit mode currently active
                if (cursor_edit == CURSOR_EDIT_HOUR) {
                    clock_time_change_hour_utc(time_change);
                }
                else {
                    clock_time_change_minute_utc(time_change);
                }
                last_edit_time = tick_us;
                gui_needs_update = true;
            }

            last_visible_time = tick_us;
        }
    }

    // disable cursor visibility after 5 seconds
    if (cursor_visible && (tick_us - last_visible_time > CURSOR_VISIBLE_TIME)) {
        cursor_visible = false;
        gui_needs_update = true;
    }
    return gui_needs_update;
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