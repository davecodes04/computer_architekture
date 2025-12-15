# About
Another project to show how to use a GUI on the 1.28 inch LCD Display,
this time with a full analog & digital clock, showing off the capabilities
of the gui-library, which is based on the lcd library.

The programming interface and capabilities are very rudimentary
in comparison to the LVGL library, but still.

It is based on
 - WaveShare's abstractions of Pico-SDK in lib/
 - Pico-SDK
 - CMake

# Board Setup
Nothing special, there's no additional hardware on the breadboard.

# Compilation
Just do
  cmake -G Ninja -BBUILD .
  cmake --build BUILD
  cp src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up
