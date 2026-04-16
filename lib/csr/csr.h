#ifndef LIB_CSR_H
#define LIB_CSR_H

#include <stdint.h>

/**
 * \brief Initialize the sampling of Instruction and Cycle counters.
 *
 * We want to sample the number of Instructions and the number of Cycles executed
 * every 500ms in sample_values_cb().
 * First we need to initialize the RISC-V Hardware Thread "hart" to NOT inhibit
 * these two counters to be counted, aka, we need to clear Bit 0 and Bit 2
 * (see PICO-datasheet page 313)
 * 
 * \return The previously set value of the CSR MCOUNTINHIBIT.
 */
uint32_t cpu_counter_init(void);

/**
 * \brief Return the number of retired instructions.
 * This requires cpu_counter_init() to be called
 * 
 * \return The number of instructions as 64-bit unsigned integer since boot.
 */
static inline uint64_t cpu_rdinstret(void) {
    uint32_t instr_l, instr_h;
    /*
     * Read the low and high 32-bits of the number of Instructions retired.
     * This is then combined to a 64-bit value (or better yet passed as two
     * registers to snprintf).
     * 
     * In order to fetch the (very unlikely) case, that the lower 32-bit overflow
     * (into the higher 32-bits), we read out the high-value twice, and repeat
     * the process, if the values are not equal.
     * We need to tell the compiler, that the register t0 is clobbered, using
     * gcc's way of specifying a register, memory is not touched, hence no
     * clobber of "memory"
     */
    asm volatile ("asm_rdinstret_again:\n\t"
                  "rdinstreth t0\n\t"          // Instead of letting the compiler choose,
                  "rdinstret  %0\n\t"          // we use the temporary register t0.
                  "rdinstreth %1\n\t"          // Hence, we need to mark it clobbered.
                  "bne        t0, %1, asm_rdinstret_again\n\t"
                  : /* Output */"=r" (instr_l), "=r" (instr_h)
                  : /* No Input */
                  : /* Clobber */ "%t0");
    return ((uint64_t)instr_h << 32) | instr_l;
}

/**
 * \brief Return the number of CPU cycles (of the core)
 * This requires cpu_counter_init() to be called
 * 
 * \return The number of cycles as 64-bit unsigned integer since boot.
 */
static inline uint64_t cpu_rdcycles(void) {
    uint32_t cycles_l, cycles_h;
    asm volatile ("asm_rdcycle_again:\n\t"
                  "rdcycleh t0\n\t"
                  "rdcycle  %0\n\t"
                  "rdcycleh %1\n\t"
                  "bne      t0, %1, asm_rdcycle_again\n\t"
                  : /* Output */"=r" (cycles_l), "=r" (cycles_h)
                  : /* No Input */
                  : /* Clobber */ "%t0");
    return ((uint64_t)cycles_h << 32) | cycles_l;
}


#endif /* LIB_CSR_H */