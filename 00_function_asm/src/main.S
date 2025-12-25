# Assembler example to show how to easily program assembler
# Documentation for GNU Assembler (and the directives below):
#   https://sourceware.org/binutils/docs/as/index.html
#
# Author: Rainer Keller, HS-Esslingen
#

# These are comment lines -- everything after # is ignored, however
# coment lines starting in column 0 could be interpreted, if the include
# a line number, followed by a dash then a string, like '# 9-17 "file.S"'

        # The following attributes are specific to RISC-V Assemblers:
        # The arch attribute tells the assembler, what kind of RISC-V core we compile for:
        # RV32I -- then with version/patch-level information on the exact specification -- here version 2.0, patch-level 1
        #   plus M extension in version 2.0, patch-level 0, and so on
        # The other files show simpler definitions (without patch-level)
        .attribute arch, "rv32i2p1_m2p0_a2p1_c2p0_zicsr2p0_zifencei2p0"
        .attribute unaligned_access, 0                     # Do not allow unaligned memory access
        .attribute stack_align, 16                         # Stack Alignment to 16 Bytes (16 is the default)

        # The C-compiler even adds a data .section
        # followed by flags here in quotation marks,
        # followed by a @type (a specific constant)
        # This is somehow necessary, since a ".section data" does not lead the linker
        # to properly load the address of "myString".
        # more https://sourceware.org/binutils/docs/as/Section.html
        .section data, "aMS", @progbits, 1                 # Declaration of the data section: _a_llocatable, _M_ergeble, contains zero-terminated _S_trings
        .align  2                                          # Aligned to 2^2 Bytes (aka 4 Bytes)
myString:                                                  # Label of the following String "myString" as local symbol
        .string "Test Assembler"                           # NUL-terminated String -- sometimes as also with GAS directory .asciiz


        .text                                              # Mark the next section as .text/.code section
        # See the description above:
        #  section .text.startup, "ax", @progbits          # ELF Section with name ".text.startup" is e_x_ecutable and a_llocatable -- and it contains "data" (code bytes)
        .align  1                                          # Aligned to 2^1 Bytes (aka 2 Bytes)
        .global  main                                      # Defines a globally-visible (exported) symbol "main", visible to the linker ld
main:                                                      # The location of the Symbol "main": here, the function starts
        # 0. Safe the callee's state: safe the return address (ra) register to the Stack -- create some space, first
        addi    sp, sp, -4                                 # Reserve 4 bytes on the Stack
        sw      ra, -4(sp)                                 # Save the ra register onto the stack

        # 1. An example how to compare real instructions (mul, lui, et al) and pseudo-instructions (li, mv, sw, ret):
        li    a0, 0xffff                                   # Load-immediate; hey bigger than 12-bit? This is pseudo-instruction will be turned into 2 instructions: lui plus addi
        mul   a1, a0, a0                                   # Multiply 65535 * 65535 into a1 ---> 4.294.836.225, aka fits into 32-bit
        li    a2, 65536                                    # This time load immediate, but here instead of 0xffff, one higher 0x10000 
        mul   a3, a2, a2                                   # Multiply 65536 * 65536 into a3 ---> will not fit into 32-Bits --> a3 = 0
        mulh  a4, a2, a2                                   # Multiply but return the HIGH-bist into a4, aka we get the "overflown" 32-bit --> a4 = 1
        mulhu a5, a2, a2                                   # Multiple high unsigned: interpret the multiplication as unsigned (same as above since 65535 > 0!)

        # 2. An example on how to call functions
        lui     a0, %hi(myString)                          # Load the upper-immediate 20 Bits of the address of Symbol "myString"
        addi    a0, a0, %lo(myString)                      # Add to that the lower 20 Bits of the address
        jal     my_strlen                                  # Jump-and-Link to function my_strlen

        # 3. Restore the state: restore the return address (ra) register, and restore the stack pointer
        lw      ra, -4(sp)                                 # Restore the ra register
        addi    sp, sp, 4                                  # Clean up the Stack-pointer

        # 5. Exit this function
        ret                                                # Return to the caller

        # The following is not really necessary, it calculates the size of the main function
        # This may be necessary for situations when You need to calculate the size of Your
        # data stored in the data section
        .size   main, . - main                             # GAS Trick: "." evaluates to the current address (after the start)

        