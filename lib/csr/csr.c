#include <stdint.h>
#include "hardware/platform_defs.h"
#include "hardware/regs/rvcsr.h"

#include "csr.h"

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
uint32_t cpu_counter_init(void) {
    uint32_t csr_value;
    /*
     * We need to access the CSR (ext. Zicsr) using the read-clear-immedate
     * instruction, passing the Offset 0x320 (defined in hardware/regs/rvcsr.h)
     * and the bits to sets both as immediate as input.
     * We as well may read the currently set value into the variable csr_value.
     * If we had a longer assembler sequence, we might need to inform GCC of any
     * "clobbered" (killed) resources, e.g. a register like "%t0" or "memory".
     */
    asm volatile ("csrrci %0, %1, %2\n\t"         // We add new-line and tab to beautify
                  : /* Output*/ "=r" (csr_value)  // The compiler replaces %0 as general register
                  : /* Input */"i" (RVCSR_MCOUNTINHIBIT_OFFSET), // The compiler replaces %1
                               "i" (0x1 << 0 | 0x1 << 2)         // and %2 as immediate values.
                  : /* No Clobber */ );
    return csr_value;
}
