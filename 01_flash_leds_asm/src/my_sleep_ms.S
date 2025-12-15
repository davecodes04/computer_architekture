        .file   "my_sleep_ms.S"
        .attribute arch, "rv32imac_zbs_zba"
        .attribute unaligned_access, 0
        .attribute stack_align, 16

        .text
        .section .text.startup,"ax",@progbits
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
        .type   my_sleep_ms, @function
my_sleep_ms:
        lui t0, SLEEP_1_MS
        mul t0, t0, a0
.my_sleep_ms_repeat:
        add t0, t0, -1
        bne t0, zero, .my_sleep_ms_repeat
        ret
        .size   my_sleep_ms, . - my_sleep_ms # GAS Trick: "." evaluates to the current address (after the start)

        .ident  "GCC: (GNU) 16.0.0 20251109 (experimental)"