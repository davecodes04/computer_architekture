/**
 * \file my_sleep_ms.S
 * \brief Function to sleep for ms multi-seconds.
 *
 * This is *not* a proper timing function, as it is just 
 * based on "experience" running on 150 MHz RP2350 with
 * this inner loop.  We will use the timers otherwise
 * for proper timing (just like the Pico-SDK employs).
 *
 * Author: Rainer Keller, HS-Esslingen
 */

        .text
        .align  1

# Define to a large enough value to denote one ms
.equ SLEEP_1_MS,   0xf
.equ SLEEP_MS,     500

/**
 * \fn void my_delay;
 * \brief Sleeps or rather busy-wait for 500 ms.
 * \return void
 */
        .global my_delay
my_delay:
        /** TODO: ADD YOUR OWN CODE */
        li      t0, 35000000                               # Load the number of ms to sleep into t0

.my_delay_loop:
        addi    t0, t0, -1                                 # Decrement the counter
        bnez    t0, .my_delay_loop                         # If not zero, loop again
        ret                                                # Return to the caller