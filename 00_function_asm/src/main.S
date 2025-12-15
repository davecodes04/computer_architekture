# Assembler example to show how to easily program assembler
# Documentation for GNU Assembler (and the directives below):
#   https://sourceware.org/binutils/docs/as/index.html
#
# Author: Rainer Keller, HS-Esslingen
#
# Remember, this is an ELF target

        .file   "main.S"                                   # Old Assembler directive to tell the file name of this unit

        # The following attributes are specific to RISC-V Assemblers:
        # The arch attribute tells the assembler, what kind of RISC-V core we compile for:
        # RV32I -- then with version/patch-level information on the exact specification -- here version 2.0, patch-level 1
        #   plus M extension in version 2.0, patch-level 0, and so on
        # The other files show simpler definitions (without patch-level)
        .attribute arch, "rv32i2p1_m2p0_a2p1_c2p0_zicsr2p0_zifencei2p0"
        .attribute unaligned_access, 0                     # Do not allow unaligned memory access
        .attribute stack_align, 16                         # Stack Alignment to 16 Bytes (16 is the default)

        .section data, "aMS", @progbits, 1                 # Declaration of the data section: _a_llocatable, _M_ergeble, contains zero-terminated _S_trings
        .align  2                                          # Aligned to 2^2 Bytes (aka 4 Bytes)
.myString:                                                 # Label of the following String ".myString" -- by convention starts with a "."
        .string "Test Assembler"                           # NUL-terminated String

        .text                                              # Mark the next section as .text/.code section
        .section .text.startup, "ax", @progbits            # ELF Section with name ".text.startup" is e_x_ecutable and a_llocatable -- and it contains "data" (code bytes)
        .align  1                                          # Aligned to 2^1 Bytes (aka 2 Bytes)
        .global  main                                      # Defines a globally-visible (exported) symbol "main", visible to the linker ld
        .type   main, @function                            # ELF: mark the symbol main as a function
main:                                                      # The location of the Symbol "main": here, the function starts
        # 0. Safe the callee's state: safe the return address (ra) register to the Stack -- create some space, first
        addi    sp, sp, -4                                 # Reserve 4 bytes on the Stack
        sw      ra, 0(sp)                                  # Save the ra register onto the stack

        # 1. an Example on how to call functions
        lui     a0, %hi(.myString)                         # Load the upper-immediate 20 Bits of the address of Symbol ".myString"
        addi    a0, a0, %lo(.myString)                     # Add to that the lower 20 Bits of the address
        jal     my_strlen                                  # Jump-and-Link to function my_strlen

        # 2. Restore the state: restore the return address (ra) register, and restore the stack pointer
        lw      ra, 0(sp)                                  # Restore the ra register
        addi    sp, sp, 4                                  # Clean up the Stack-pointer

        # 4. Exit this function
        ret                                                # Return to the caller

        .size   main, . - main                             # GAS Trick: "." evaluates to the current address (after the start)

        .ident  "GCC: (GNU) 16.0.0 20251109 (experimental)"
        .section .note.GNU-stack, "", @progbits

        