.section .rodata
        .align 0
H2A:                                    # Unsere Lookup-Tabelle im ROM
        .ascii "0123456789ABCDEF"       # .ascii hängt (im Gegensatz zu .asciz) KEIN NUL-Byte an

        .text
        .align 1
        .global hexToASCII

/**
 * \fn void hexToASCII(char * string, unsigned int val);
 * \brief Converts a 32-bit hex value to an ASCII string (0x...).
 *
 * \param[in] a0 Pointer to the destination string in RAM.
 * \param[in] a1 The 32-bit unsigned integer value.
 * \return void
 * \note Modified registers: t0, t1, t2, t3, t4, t5
 */
hexToASCII:
        # 1. "0x" an den Anfang des Strings schreiben
        li      t0, 48                  # ASCII-Wert für '0'
        sb      t0, 0(a0)               # string[0] = '0'
        
        li      t0, 120                 # ASCII-Wert für 'x'
        sb      t0, 1(a0)               # string[1] = 'x'

        # 2. Schleife vorbereiten
        la      t4, H2A                 # t4 = Basisadresse der H2A Tabelle
        addi    t5, a0, 2               # t5 = Zieladresse im String (startet bei string[2])
        li      t0, 28                  # t0 = Start-Shiftwert (beginnt bei 28)

.hex_loop:
        # Array-Index berechnen: (val >> shift) & 0xF
        srl     t1, a1, t0              # t1 = val >> shift (Logischer Shift nach rechts)
        andi    t1, t1, 0xF             # t1 = t1 & 0xF (Maskiere die unteren 4 Bits)
        
        # ASCII-Zeichen aus H2A Tabelle laden: H2A[index]
        add     t2, t4, t1              # t2 = Adresse von H2A + index
        lbu     t3, 0(t2)               # t3 = Lade das Zeichen aus der Tabelle
        
        # Zeichen in den Zielstring schreiben
        sb      t3, 0(t5)               # Speichere das Zeichen
        
        # Für den nächsten Schleifendurchlauf aktualisieren
        addi    t5, t5, 1               # Zieladresse um 1 erhöhen (string[i+1])
        addi    t0, t0, -4              # Shiftwert um 4 verringern (28 -> 24 -> 20 ...)
        
        # Wenn der Shiftwert noch >= 0 ist, Schleife wiederholen
        bgez    t0, .hex_loop

        # 3. NUL-Byte ans Ende hängen
        li      t0, 0
        sb      t0, 0(t5)               # string[10] = '\0'

        ret