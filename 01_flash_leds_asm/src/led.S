/**
 * \file led.S
 * \brief Functions to handle LEDs connected to GPIOs using SIO
 *
 * These model the way, the Pico-SDK initialized a GPIO for
 * Single-Cycle IO (SIO) following the hardware definitions in
 * the various HW-definition files (most importantly the CMSIS),
 * as well as their interaction with he IO banks in a PAD.
 *
 * Author: Rainer Keller, HS-Esslingen
 */

/**
 * First we require some constant definitions, to make the ASM code
 * more readable.
 */
 
# From pico-sdk/src/rp2_common/cmsis/stub/CMSIS/Device/RP2350/Include/RP2350.h
.equ SIO_BASE,        0xD0000000 # The start address of the Single-Cycle IO Block
.equ IO_BANK0_BASE,   0x40028000 # 
.equ PADS_BANK0_BASE, 0x40038000 # User Bank Pad Control register

# From pico-sdk/src/rp2350/hardware_regs/include/hardware/regs/pads_bank0.h
.equ PADS_BANK0_GPIO0_IE_BITS,    0x40 // Input enable
.equ PADS_BANK0_GPIO0_OD_BITS,    0x80 // Output disable bit. Has priority over output enable from peripherals
.equ PADS_BANK0_GPIO0_ISO_BITS,  0x100 // Pad isolation control. Remove this once the pad is configured by software.

.equ NUM_BANK0_GPIOS, 32          # RP2350A has 32 GPIOs in BANK0, only RP2350B has more GPIOs

# Enum gpio_function_t from pico-sdk/src/rp2350/hardware_structs/include/hardware/structs/io_bank0.h
.equ GPIO_FUNC_SIO,             5 # ///< Select SIO as GPIO pin function

# From https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf#tab-registerlist_sio
.equ GPIO_OUT_SET,           0x18 # GPIO0..31 output value set
.equ GPIO_OUT_CLR,           0x20 # GPIO0..31 output clear
.equ GPIO_OE_SET,            0x38 # GPIO0..31 output enable set
.equ GPIO_OE_CLR,            0x40 # GPIO0..31 output enable clear

/*
 * Following is the .text/code section
 */

        .text
        .align  1

/**
 * \fn void led_init(int gpio);
 * \brief Initialize the a GPIO port for an LED
 *
 * Initialize the given parameter gpio as LED port, aka as SIO output.
 *
 * \param[in] gpio The gpio port (in register a0) to initialized as SIO output
 * \return void     
 */
        .global led_init
led_init:
        # Save Registers s0-sX, ra
        li      t0, NUM_BANK0_GPIOS                        # Get the number of GPIOs in Bank0 (in RP2350A, it's just one bank)
        bge     a0, t0, .led_init_out                      # and jump out if the gpio parameter is bigger
        bset    a3, zero, a0                               # Set the bit mask a3=(1<<gpio)

        lui     a4, %hi(SIO_BASE)                          # Load base address of Single-Cycle IO -- no need to addi %lo, since it's zero
        sw      a3, GPIO_OE_CLR(a4)                        # Store the mask to GPIO_OE_CLR
        sw      a3, GPIO_OUT_CLR(a4)                       # Store the mask to GPIO_OUT_CLR

        lui     a5, %hi(PADS_BANK0_BASE)                   # Load base address of PADS_BANK0_BASE
        addi    a5, a5, 4                                  # The GPIOs start 4 bytes later
        sh2add  a5, a0, a5                                 # Shift (gpio << 2) and add to a5
        lw      a4, 0(a5)                                  # load pads_bank0_hw->io[gpio]
        lui     a3, 0x1                                    # a3 = 0x1000 (4096)
        add     a3, a3, a5

        xori    a4, a4, PADS_BANK0_GPIO0_IE_BITS           # Set input enable on, output disable off
        andi    a4, a4, (PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS)
        sw      a4, 0(a3)

        lui     a2, %hi(IO_BANK0_BASE)                     # Load base address of IO_BANK0_BASE, again no need add addi since lower 12 bits are 0
        sh3add  a1, a0, a2                                 # Shift (gpio << 3) and add a2
        li      a4, GPIO_FUNC_SIO                          # Load the fn
        sw      a4, 4(a1)                                  # io_bank0_hw->io[gpio].ctrl = fn

        lui     a3, 0x3                                    # a3 = 0x3000 (12 kB, 12.288)
        add     a5, a5, a3                                 # XXX ???
        li      a4, PADS_BANK0_GPIO0_ISO_BITS              #
        sw      a4, 0(a5)                                  # hw_clear_bits(&pads_bank0_hw->io[gpio], PADS_BANK0_GPIO0_ISO_BITS);

        # Now gpio_set_dir(gpio, GPIO_OUT);
        lui     a4, %hi(SIO_BASE)                          # Load base address of Single-Cycle IO -- no need to addi %lo, since it's zero
        bset    a3, zero, a0                               # Set the bit mask a3 = (1<<gpio)
        sw      a3, GPIO_OE_SET(a4)                        # sio_hw->gpio_oe_set = mask;
.led_init_out:
        ret


/**
 * \fn void led_on(int gpio);
 * \brief Turn on the LED (set the GPIO output)
 *
 * Sets the given gpio port to 1.
 *
 * \param[in] gpio The gpio port (in register a0) to set to 1
 * \return void
 */
        .global led_on
led_on:
        li      t0, NUM_BANK0_GPIOS                        # Get the number of GPIOs in Bank0 (in RP2350A, it's just one bank)
        bge     a0, t0, .led_on_out                        # and jump out if the gpio parameter is bigger

        bset    a1, zero, a0                               # Set the bit mask a1 = (1 << gpio)
        lui     a2, %hi(SIO_BASE)                          # Load the base address of SIO_BASE, no need to addi since lower 12 bits are 0
        sw      a1, GPIO_OUT_SET(a2)                       # Set the Bit of the in sio_hw->gpio_set
.led_on_out:
        ret


/**
 * \fn void led_off(int gpio);
 * \brief Turn off the LED (unset the GPIO output)
 *
 * Sets the given gpio port to 0.
 *
 * \param[in] gpio The gpio port (in register a0) to set to 0
 * \return void
 */
        .global led_off
led_off:
        li      t0, NUM_BANK0_GPIOS                        # Get the number of GPIOs in Bank0 (in RP2350A, it's just one bank)
        bge     a0, t0, .led_off_out                       # and jump out if the gpio parameter is bigger

        bset    a1, zero, a0                               # Set the bit mask a1 = (1 << gpio)
        lui     a2, %hi(SIO_BASE)                          # Load the base address of SIO_BASE, no need to addi since lower 12 bits are 0
        sw      a1, GPIO_OUT_CLR(a2)                       # Set the Bit of the in sio_hw->gpio_clr
.led_off_out:
        ret
