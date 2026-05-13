/** TODO: ADD YOUR OWN DATA section */
        .section .rodata                                   # marks the section as read only data
        .align 2                                           
test_string:                                               
        .asciz "Hallo.."                                   
        
        .section .data                                     # marks the section as read and write
        .align 2
        .global main
dest_string:
        .space 50                                          # reserves 50 bytes for the destination string
        
        .text                                              # Mark the next section as .text/.code section
        .align  1                                          # Aligned to 2^1 Bytes (aka 2 Bytes)
        .global  main                                      # Defines a globally-visible (exported) symbol "main", visible to the linker ld
main:                                                      # The location of the Symbol "main": here, the function starts
        # 0. Safe the used callee-saved registers ra, s0-s2 onto the stack
        addi    sp, sp, -12                                # Allocate data on the stack: 12 Bytes
        sw      ra, 8(sp)                                  # Store the return address (ra) register on the stack
        sw      s1, 4(sp)                                  # Store the saved register s1 on the stack
        sw      s2, 0(sp)                                  # Store the saved register s2 on the stack

        /** TODO: ADD YOUR OWN CODE */
        
        # Test string length
        la a0, test_string                                 # Load the address of the string "Hallo" into register a0
        jal my_strlen                                      # Call the function my_strlen, which calculates the length of the string and returns it in register a0

        # Test string copy
        la a0, dest_string                                 # load the address of the destination string into a0 
        la a1, test_string                                 # load the address of the string "Hallo" into a1
        jal my_strcpy                                      # call the function my_strcpy which copies the original string and returns it in the register a0
        # x/-5b $a0 in der debug console um um 5 Byte rückwärts an der Speicheradresse a0 zu gehen

        # Test to lower
        la      a0, dest_string                            # pointer to the RAM string stored in a0
        jal     my_strtolower
        # x/s &dest_string

        # Test for hexToASCII conversion
        li      s2, 0                                      # our 16 bit counter (s2) starts at 0 # for test flip from 65553 to 0 set value to 65535
        # x/s &dest_string

        # Test for decToASCII conversion
        la      a0, dest_string                            # pointer to the RAM string stored in a0
        # x/s &dest_string

        # Test 1 dec
        li      a1, 256                                    # load 256 as first test case
        jal     decToASCII                                 # jump and link to the conversion function
        # set breakpoint here

        # Test 2 dec
        la      a0, dest_string                            # load in the pointer to the RAM again
        li      a1, -2147483648                            # this value is interesting because we have to turn it from negative to positive in the middle of the conversion 
                                                           # and if we do that poorly then the CPU crashes because the value is one bit too long than what is supported
        jal     decToASCII                                 # jump and link to the conversion function
        # set breakpoint here
.test_loop:
        # set parameters for hexToASCII
        la      a0, dest_string                            # pointer to RAM string
        mv      a1, s2                                     # load the counter into a1
        # li    a1, 0xF018                                 # load 0xF018 for testing as in the PDF
        jal     hexToASCII                  

        addi    s2, s2, 1                                  # increment counter
        
        lui     t0, 0x10                                   # constrain the counter to 16-bits to simulate a rollover.
        addi    t0, t0, -1                                 # we create a bitmask for 0xFFFF (65535) by loading 0x10000 and subtracting 1,
        and     s2, s2, t0                                 # then apply it to the counter using a bitwise AND.

        j       .test_loop                                 # loop forever

.main_loop:
        j       .main_loop                                 # Loop forever

        # ####################################### #
        # In fact, the following clean-up code will never be executed... Still keep it, to show best practice
        lw      s2, 0(sp)                                  # Restore the saved register s2 from the stack
        lw      s1, 4(sp)                                  # Restore the saved register s1 from the stack
        lw      ra, 8(sp)                                  # Restore the saved register ra from the stack
        addi    sp, sp, 12                                 # Clean up the stack-pointer
        ret                                                # Return to the caller