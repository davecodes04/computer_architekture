        .text
        .align 1
        .global my_strlen
        .global my_strcpy
        .global my_strtolower
/**
* \fn my_strlen(const char *str);
* \brief Calculate the length of a string
*
* \param[in] str The string to calculate the length of
* \return The length of the string
* \note Modified registers: t0, t1, a0
*/
my_strlen:
    li t0, 0                                # Initialize the length counter (t0) to 0
    j .strlen_loop                          # jump to the actual loop

.strlen_loop:
    lbu t1, 0(a0)                           # Load the next byte of the string into t1
    beqz t1, .strlen_end                    # jump to strlen_done if the 0 termiantor has been reached
    addi t0, t0, 1                          # add 1 to the length counter register t0
    addi a0, a0, 1                          # Move to the next byte of the string
    j .strlen_loop                          # repeat if not at the end of the string

.strlen_end:
    mv a0, t0                               # move the length of the string to a0
    ret

/**
* \fn my_strcpy(const char *str);
* \brief Calculate the length of a string
*
* \param[in] str The string to calculate the length of
* \return The length of the string
* \note Modified registers: t0, t1, a0
*/
my_strcpy:
    lbu t0, 0(a1)                           # load first Byte from a0 to t0
    sb t0, 0(a0)                            # save that Byte from t0 to the destination register a0 (offset 0)
    beqz t0, .my_strcpy_end                 # branch if t0 is 0 i.e. at the end of the string
    addi a0, a0, 1                          # increment a0 so that its pointing to the next spot for the next letter
    addi a1, a1, 1                          # increment a1 so that the next letter is pointed at
    j my_strcpy                             # jump back to the beginning
    
.my_strcpy_end:                             # if it is done, then return
    ret

/**
 * \fn int my_strtolower(char * string);
 * \brief Converts all uppercase characters in a string to lowercase.
 *
 * \param[in] a0 Pointer to the NUL-terminated string in RAM.
 * \return The length of the string in register a0.
 * \note Modified registers: t0, t1, t2, t3, a0
 */
my_strtolower:
        li      t0, 0                       # Längenzähler auf 0 setzen
        li      t2, 65                      # ASCII-Wert für 'A' (Untergrenze) in t2
        li      t3, 90                      # ASCII-Wert für 'Z' (Obergrenze) in t3

.strtolower_loop:
        lbu     t1, 0(a0)                   # Lade das aktuelle Zeichen (Byte) in t1
        beqz    t1, .strtolower_end         # Wenn das Zeichen NUL (0) ist, sind wir am Ende

        # Ist das Zeichen ein Großbuchstabe? (Zwischen 65 und 90)
        blt     t1, t2, .skip_convert       # Wenn kleiner als 65 ('A'), überspringen
        bgt     t1, t3, .skip_convert       # Wenn größer als 90 ('Z'), überspringen

        # Es ist ein Großbuchstabe! Umwandlung in Kleinbuchstabe:
        # Wir addieren 32 (alternativ: ori t1, t1, 0x20)
        addi    t1, t1, 32
        
        # Speichere das geänderte Zeichen zurück in den Arbeitsspeicher (RAM)
        sb      t1, 0(a0)

.skip_convert:
        addi    t0, t0, 1                   # Längenzähler um 1 erhöhen
        addi    a0, a0, 1                   # Pointer um 1 verschieben (nächstes Zeichen)
        j       .strtolower_loop            # Zurück zum Anfang der Schleife

.strtolower_end:                            # x/-7b $a0 if you set the breakpoint here then you can see the converted string
        mv      a0, t0                      # Verschiebe die berechnete Länge nach a0 (Rückgabewert)
        ret                                 # Zurück zur aufrufenden Funktion (main)