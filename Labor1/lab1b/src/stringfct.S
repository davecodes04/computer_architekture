        .text
        .align 1
        .global my_strlen
        .global my_strcpy
        .global my_strtolower
/**
* \fn my_strlen(const char *str);
* \brief Calculate the length of a string
*
* \param[in] a0 pointer to the NUL terminated string
* \return The length of the string
* \note Modified registers: t0, t1, a0
*/
my_strlen:
    li t0, 0                                # initialize length counter

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
* \fn my_strcpy(char *dest, const char *src);
* \brief Copies the string from the source to the destination
*
* \param[in] a0 pointer to the destination buffer
* \param[in] a1 pointer to the source string
* \return void
* \note Modified registers: t0, a0, a1
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
        li      t0, 0                       # initialize length counter
        li      t2, 65                      # ASCII value for 'A' 
        li      t3, 90                      # ASCII value for 'Z'

.strtolower_loop:
        lbu     t1, 0(a0)                   # load current character byte from the string
        beqz    t1, .strtolower_end         # branch if NUL terminator is hit

        blt     t1, t2, .skip_convert       # if its less than 'A', skip
        bgt     t1, t3, .skip_convert       # if its more than 'Z', skip

        addi    t1, t1, 32                  # add 32 so the uppercase is converted to lowercase
        
        sb      t1, 0(a0)                   # store the value in RAM

.skip_convert:
        addi    t0, t0, 1                   # increment string length counter
        addi    a0, a0, 1                   # increment string pointer to look at the next character
        j       .strtolower_loop            # loop

.strtolower_end:                            # x/-7b $a0 if you set the breakpoint here then you can see the converted string
        mv      a0, t0                      # move length into a0 as return value
        ret