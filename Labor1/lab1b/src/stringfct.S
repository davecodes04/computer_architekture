        .text
        .align 1
        .global my_strlen
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