/** TODO: ADD YOUR OWN DATA section */
.section .rodata                                           # Markiert einen Bereich für Konstanten (Read-Only)
        .align 2                                           # Speicherausrichtung
test_string:                                               # <-- Das ist dein Label (die Adresse)
        .asciz "Hallo"                                     # .asciz legt den String an UND hängt automatisch die Null (NUL-Byte) an!
        
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
        la a0, test_string                                  # Load the address of the string "Hallo" into register a0
        jal my_strlen                                       # Call the function my_strlen, which calculates the length of the string and returns it in register a0

.main_loop:
        j       .main_loop                                 # Loop forever

        # ####################################### #
        # In fact, the following clean-up code will never be executed... Still keep it, to show best practice
        lw      s2, 0(sp)                                  # Restore the saved register s2 from the stack
        lw      s1, 4(sp)                                  # Restore the saved register s1 from the stack
        lw      ra, 8(sp)                                  # Restore the saved register ra from the stack
        addi    sp, sp, 12                                 # Clean up the stack-pointer
        ret                                                # Return to the caller
        
