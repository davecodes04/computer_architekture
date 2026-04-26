.text
        .align 1
        .global decToASCII

/**
 * \fn void decToASCII(char * string, int val);
 * \brief Converts a 32-bit signed decimal value to an ASCII string.
 *
 * \param[in] a0 Pointer to the destination string in RAM.
 * \param[in] a1 The 32-bit signed integer value.
 * \return void
 * \note Modified registers: t0, t1, t2, t3, t4, a0, a1
 */
decToASCII:
        li      t0, 32                              # ASCII for ' '
        bgez    a1, .is_positive                    # Branch if Greater or Equal Zero
        
        li      t0, 45                              # ASCII for '-'
        neg     a1, a1                              # positive for the subsequent division algorithm

.is_positive:
        sb      t0, 0(a0)                           # save ' ' or '-' at string[0]
        addi    a0, a0, 1                           # move over by one for the numbers

        li      t0, 1000000000                      # start-divisor (10^9)
        li      t4, 10                              # set the divider 
        li      t3, 9                               # set loop counter

.dec_loop:
        divu    t1, a1, t0                          # unsigned division: t1 (quotient) = a1 (current value) / t0 (current divisor)
        remu    a1, a1, t0                          # unsigned remainder: a1 = a1 % t0
        
        addi    t2, t1, 48                          # ASCII value = quotient + 48 ('0')
        sb      t2, 0(a0)                           # stores the ASCII byte in the string
        
        addi    a0, a0, 1                           # increment the string pointer
        divu    t0, t0, t4                          # divide the divisor (t0) by 10 (t4)
        
        addi    t3, t3, -1                          # decrement the loop iteration counter t3
        bnez    t3, .dec_loop                       # loop while t3 is not 0

        addi    t2, a1, 48                          # add 48 to the final remainder left in a1 to convert it to ASCII
        sb      t2, 0(a0)                           # store that character in the string

        li      t0, 0                               # NUL terminator
        sb      t0, 1(a0)                           # load the NUL terminator in the string

        ret