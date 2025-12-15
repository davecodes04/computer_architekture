        .file   "my_strlen.S"
        .attribute arch, "rv32imac_zbs_zba"
        .attribute unaligned_access, 0
        .attribute stack_align, 16

        .text
        .section        .text.startup,"ax",@progbits
        .align  1

/**
 * \fn int my_strlen(const char * string);
 * \brief Return the string's length
 *
 * Searches the string passed as ASCIIZ string for the NUL Byte
 * and returns the length.
 *
 * \param[in] string The string (in register a0)
 * \return The length of the string (in register a0)
 */
        .global my_strlen
        .type   my_strlen, @function
my_strlen:
        # Since we do not call any other function (thereby clobbering ra),
        # nor change the values of any callee-saved register, we do not
        # need to save any state on the Stack
        mv      t0, a0             # Copy first argument to t0 = points to string
        beq     t0, zero, .L1.out  # If NULL pointer, exit
.L0.next:                          # Per convention, start Label with .L<LABEL#>.<SOME_NAME>
        lbu     t1, 0(t0)          # Load first character
        beq     t1, zero, .L1.out  # If character is NUL byte, exit
        addi    t0, t0, 1          # Add 1, next byte
        j       .L0.next
.L1.out:
        sub     a0, t0, a0         # Subtract t0 - a0, and return in a0
        ret                        # Return to caller
        .size   my_strlen, . - my_strlen # GAS Trick: "." evaluates to the current address (after the start)

        .ident  "GCC: (GNU) 16.0.0 20251109 (experimental)"