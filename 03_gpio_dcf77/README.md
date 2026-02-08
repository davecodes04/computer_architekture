# About
This example shows how to use a GPIO to sample
the DCF77 antenna.

It is based on
 - WaveShare's abstractions of Pico-SDK in lib/
 - Pico-SDK
 - CMake

# Board Setup
Please attach the provided DCF77 antenna:
 - the red cable (+) to Vcc (the plus rail)
 - the black cable (-) to GND (the minus rail)
 - the yellow cable to GND as well (if always enabled)
   or to a GPIO (set as output) to dynamically dis- and enable
 - the green cable to a GPIO (XXX) or to a ADC-capable GPIO
   to measure Voltages of the signal.


XXXXX; please fix the code


# Compilation
Just do
  cmake -G Ninja -BBUILD .
  cmake --build BUILD
  cp src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

