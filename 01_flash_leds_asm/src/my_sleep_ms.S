/** \file my_sleep_ms.S
 * \brief Function to sleep for ms multi-seconds.
 *
 * This is *not* a proper timing function, as it is just 
 * based on "experience" running on 150 MHz RP2350 with
 * this inner loop.  We will use the timers otherwise
 * for proper timing (just like the Pico-SDK employs).
 */

        .text
        .align  1

# Define to a large enough value to denote one ms
.equ SLEEP_1_MS,   0xf

/**
 * \fn void my_sleep_ms(int ms);
 * \brief Sleeps or rather busy-waits for ms milliseconds.
 *
 * Simple loop to busy-wait for ms milliseonds.
 *
 * \param[in] ms The number of milli-seconds to sleep
 * \return void
 */
        .global my_sleep_ms
my_sleep_ms:
        lui t0, SLEEP_1_MS
        mul t0, t0, a0
.my_sleep_ms_repeat:
        add t0, t0, -1
        bne t0, zero, .my_sleep_ms_repeat
        ret
