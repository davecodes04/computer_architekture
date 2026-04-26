.section .rodata
        .align 0
H2A:                                    
        .ascii "0123456789ABCDEF"               # ASCII table in ROM

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
        li      t0, 48                          # ASCII value for '0'
        sb      t0, 0(a0)                       # string[0] = '0'
        
        li      t0, 120                         # ASCII value for 'x'
        sb      t0, 1(a0)                       # string[1] = 'x'

        la      t4, H2A                         # t4 = base memory address of our lookup table H2A
        addi    t5, a0, 2                       # t5 = initialize write pointer starting at string[2]
        li      t0, 28                          # t0 = initialize shift value starting at 28

.hex_loop:
        srl     t1, a1, t0                      # shift input value a1 to the right
        andi    t1, t1, 0xF                     # apply bitwise AND with 0xF to mask everything but the last 4 bits
        
        add     t2, t4, t1                      # add isolated nibble value to base address of the look up table
        lbu     t3, 0(t2)                       # load corresponding ASCII character
        
        sb      t3, 0(t5)                       # store the ASCII value into destination string
        
        addi    t5, t5, 1                       # increment destination string to the next position
        addi    t0, t0, -4                      # secrement the shift amount by 4
        
        bgez    t0, .hex_loop                   # loop while shift value is greater or equal 0

        li      t0, 0                           # NUL terminator
        sb      t0, 0(t5)                       # string[10] = '\0'

        ret