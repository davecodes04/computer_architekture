# Simple Assembler implementation of String Length
# showing best practices on how to code & document
# Assembler files in RISC-V Assembler
#
# Author: Rainer Keller, HS-Esslingen
#
        .text
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
my_strlen:
        # Since we do not call any other function (which would then clobber ra),
        # nor change the values of any callee-saved registers, we do not need
        # to save any state on the Stack.
        mv      t0, a0             # Copy first argument to t0 = points to string
        beqz    t0, .L1.out        # If NULL pointer, exit
.L0.next:                          # Per convention, start local Label with .L<LABEL#>.<SOME_NAME>
        lbu     t1, 0(t0)          # Load first character
        beqz    t1, .L1.out        # If character is NUL byte, exit
        addi    t0, t0, 1          # Add 1, next byte
        j       .L0.next
.L1.out:
        sub     a0, t0, a0         # Subtract t0 - a0, and return in a0
        ret                        # Return to caller
