# Assembler example to show how to easily program assembler
# Documentation for GNU Assembler (and the directives below):
#   https://sourceware.org/binutils/docs/as/index.html
#
# Author: Rainer Keller, HS-Esslingen
#
# Remember, this is an ELF target

        .section const_section, "a"                        # Here comes a true read-only (const) data section for LED_PINS
        .align  2                                          # Aligned to 2^2 Bytes (aka 4 Bytes)
        .global .LED_PINS
        .type   .LED_PINS, @object                         # Mark the symbol as being a data object
.LED_PINS:                                                 # Define the array with the LED's GPIO pins
        .word   5
        .word   4
        .word   3
        .word   2
        .word   1
        .word   0
        .word   13
        .word   14
        .word   15
        .word   16
        .word   17
        .word   18
        .word   19
        .word   20
        .word   21
        .word   22
        .word   28
        .word   27
        .word   26
.equ    LED_PINS_BYTES, (. -.LED_PINS)                     # Constant! GAS trick: "." stores the current address (aka offset from beginning) in Bytes

.equ    SLEEP_MS, 500                                      # Sleep for 500ms

        .text                                              # Mark the next section as .text/.code section
        .align  1                                          # Aligned to 2^1 Bytes (aka 2 Bytes)
        .global  main                                      # Defines a globally-visible (exported) symbol "main", visible to the linker ld
main:                                                      # The location of the Symbol "main": here, the function starts
        # 0. Safe the used callee-saved registers ra, s1-s3 onto the stack
        addi    sp, sp, -16                                # Allocate data on the stack
        sw      ra, 12(sp)                                 # Store the return address (ra) register on the stack
        sw      s0, 8(sp)                                  # Store the saved register s0 on the stack
        sw      s1, 4(sp)                                  # Store the saved register s1 on the stack
        sw      s2, 0(sp)                                  # Store the saved register s2 on the stack

        # 1. Initialization: Use s1 as pointer to LED_PINS, while s2 is the end-pointer (both unchanged below)
        lui     s2, %hi(.LED_PINS)                         # Initialize with Pointer to first element by loading upper 20-bit
        addi    s2, s2, %lo(.LED_PINS)                     # and adding the lower 12 bit with GAS macros %hi() and %lo()
        addi    s0, s2, LED_PINS_BYTES                     # Initialize with end address
        
        # 2. Initialize GPIOs
        mv      s1, s2                                     # Initialize the pointer-variable
.main_repeat_init:
        lw      a0, 0(s1)                                  # Load the word at s1[0], aka dereference s1 into a0
        jal     led_init                                   # Jump-and-link to function led_init
        addi    s1, s1, 4                                  # Increment pointer by four --> next word
        bne     s1, s0, .main_repeat_init                  # Unless, the pointer is the end address --> repeat

        # 3. Loop forever and blink these LEDs
.main_loop:
        mv      s1, s2                                     # Re-Initialize the pointer-variable
.main_loop_all_on:
        lw      a0, 0(s1)                                  # Load the word at s1[0], aka dereference s1 into a0
        jal     led_on                                     # Call function led_on
        addi    s1, s1, 4                                  # Increment pointer by four -> next word
        bne     s1, s0, .main_loop_all_on                  # Unless, the pointer is the end address --> repeat

        # Busy loop for 500ms
        li      a0, SLEEP_MS                               # First parameter 500 ms
        jal     my_sleep_ms                                # Call my_sleep_ms

        mv      s1, s2                                     # Re-Initialize the pointer-variable
.main_loop_all_off:
        lw      a0, 0(s1)                                  # Load the word at s1[0], aka dereference s1 into a0
        jal     led_off                                    # Call function led_off
        addi    s1, s1, 4                                  # Increment pointer by four -> next word
        bne     s1, s0, .main_loop_all_off                 # Unless, the pointer is the end address --> repeat

        # Busy loop for 500ms
        li      a0, SLEEP_MS                               # First parameter 500 ms
        jal     my_sleep_ms                                # Call my_sleep_ms

        j       .main_loop                                 # Loop forever

        # ####################################### #
        # In fact, the following clean-up code will never be executed... Still keep it, to show best practice
        lw      s2, 0(sp)                                  # Restore the saved register s2 from the stack
        lw      s1, 4(sp)                                  # Restore the saved register s1 from the stack
        lw      s0, 8(sp)                                  # Restore the saved register s0 from the stack
        lw      ra, 12(sp)                                 # Restore the saved register ra from the stack
        addi    sp, sp, 16                                 # Clean up the stack-pointer
        ret                                                # Return to the caller
        