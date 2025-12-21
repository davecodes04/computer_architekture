# About
This project introduces Assembler programming in GNU `as` (or short `gas`)
assembler syntax.

It has a main() function and a prototypical implementation of other
functionality in a separate Assembler file, showing how to best document
assembler functions.  Comments may be C-style as in multi-line /* ... */
also using Doxygen-like formatting for high-lighting parameters and
return values.

Moreover the main functions shows a bit how to explore assembler
instructions -- and shows helpful pseudeo-instructions like `mv` and `li`.

This is not based on any Pico-SDK calls -- and solely requires
Pico-SDK to correctly link the file into an ELF-based executable,
which in turn is then reprogrammed into a USB-Flashing Format UF2.
Hence, we need the pico_runtime link-target dependency.

# Board Setup
Nothing special, there's no additional hardware on the breadboard.

# Compilation
Just do
  cmake -G Ninja -BBUILD .
  cmake --build BUILD
  cp src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

