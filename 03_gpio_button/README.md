# About
This example shows how to use GPIO's pull-up, pull-down
resistors -- and the Bug E9, which is in all RP2350 A2 stepping chips.

It also shows how to "debug" the setup of the GPIO SIO settings.

It is based on
 - WaveShare's abstractions of Pico-SDK in lib/
 - Pico-SDK
 - CMake

# Board Setup
Please connect the button according to the Fritzing layout
and setup the code according to your board.

# Compilation
Just do
  cmake -G Ninja -Bbuild .
  cmake --build build
  cp build/src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

