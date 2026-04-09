# About
This example provides the same functionality as the 01_flash_leds,
however it is implemented in RISC-V Assembler (for RP2350A).
It shows the best practices on how to code for RISC-V Assembler
using the GNU as Assembler (or short gas), including how to
specify the RISC-V processor architecture (in order to allow
shorter instructions like the `zbs` extension), or how to
specify arrays, how to call functions etc.

It does *not* need the Pico-SDK per se, other than for linking
purposes. Nevertheless, using the CMake-magic of Pico-SDK, the
main sources like Pico-runtime etc are compiled and linked, however
they are not strictly necessary.

It is based on
 - CMake

# Board Setup
Please attach an LED with a resistor in series in order not to
damage the LED: it has very little resistance itselve and a 
steep forward-voltage to forward-current ratio.

Please note: the Fritzing file / PNG shows a single LED, however
to be certain that any attached LED is turned on, all the available
GPIOs are setup and flashed.


# Compilation
Just do
  cmake -G Ninja -Bbuild .
  cmake --build build
  cp build/src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

