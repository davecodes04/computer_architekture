# About
This project introduces Assembler programming in GNU as (or short gas)
assembler syntax.

It has a main-function and a prototypical implementation of other
functionality in a separate Assembler file.

This is not based on any Pico-SDK calls -- and solely requires
Pico-SDK to correctly link the file into an ELF-based executable,
which in turn is then reprogrammed into a USB-Flashing Format UF2.

# Board Setup
Nothing special, there's no additional hardware on the breadboard.

# Compilation
Just do
  cmake -G Ninja -BBUILD .
  cmake --build BUILD
  cp src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

