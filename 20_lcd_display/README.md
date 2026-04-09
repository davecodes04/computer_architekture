# About
Small project to display colors on the 1.28 inch LCD Display,
very rudimentary in comparison to the LVGL library.
It is based on
 - WaveShare's abstractions of Pico-SDK in lib/
 - Pico-SDK
 - CMake

# Board Setup
Nothing special, there's no additional hardware on the breadboard.

# Compilation
Just do
  cmake -G Ninja -Bbuild .
  cmake --build build
  cp build/src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up
