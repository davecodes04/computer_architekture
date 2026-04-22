/** TODO: ADD YOUR OWN DATA section */
        .section .rodata                                   # ROM (Read-Only)
        .align 2                                           # Speicherausrichtung
test_string:                                               # <-- Das ist dein Label (die Adresse)
        .asciz "Hallo.."                                     # .asciz legt den String an UND hängt automatisch die Null (NUL-Byte) an!
        
        .section .data                                     # RAM
        .align 2
        .global main
dest_string:
        .space 100                                         # reserves 100 bytes for our copied string
        
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
        la a0, test_string                                # Load the address of the string "Hallo" into register a0
        jal my_strlen                                     # Call the function my_strlen, which calculates the length of the string and returns it in register a0

        # Test string copy
        la a0, dest_string                                 # load the address of the destination string into a0 
        la a1, test_string                                 # load the address of the string "Hallo" into a1
        jal my_strcpy                                      # call the function my_strcpy which copies the original string and returns it in the register a0
        # x/-5b $a0 in der debug console um um 5 Byte rückwärts an der Speicheradresse a0 zu gehen

        # Test to lower
        la      a0, dest_string                            # Übergib den Pointer zum RAM-String
        jal     my_strtolower

        # Test for hex to ASCII conversion
        li      s2, 0                                      # Unser 16-Bit Zähler (s2) beginnt bei 0

.test_loop:
        # Parameter für hexToASCII übergeben
        la      a0, dest_string   # Pointer auf den RAM-String
        mv      a1, s2            # Zähler als Wert übergeben
        jal     hexToASCII
        
        # HIER KANNST DU DEINEN BREAKPOINT SETZEN
        # (Im Debugger prüfen: Ist dest_string korrekt?)                    

        # Zähler um 1 erhöhen
        addi    s2, s2, 1
        
        # Zähler auf 16-Bit begrenzen (0 bis 65535) - Rollover!
        # 65535 ist hexadezimal 0xFFFF
        lui     t0, 0x10          # Lade 0x10000 in t0
        addi    t0, t0, -1        # t0 ist jetzt 0xFFFF (Die Maske für 16 Bit)
        and     s2, s2, t0        # Zähler mit Maske abschneiden

        j       .test_loop        # Unendliche Testschleife

.main_loop:
        j       .main_loop                                 # Loop forever

        # ####################################### #
        # In fact, the following clean-up code will never be executed... Still keep it, to show best practice
        lw      s2, 0(sp)                                  # Restore the saved register s2 from the stack
        lw      s1, 4(sp)                                  # Restore the saved register s1 from the stack
        lw      ra, 8(sp)                                  # Restore the saved register ra from the stack
        addi    sp, sp, 12                                 # Clean up the stack-pointer
        ret                                                # Return to the caller