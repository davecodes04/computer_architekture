# About
This example shows how to use the GPIO's in Single-Cycle IO (SIO)
mode to turn on/off LEDs.

It is based on
 - WaveShare's abstractions of Pico-SDK in lib/
 - Pico-SDK
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
  cmake -G Ninja -BBUILD .
  cmake --build BUILD
  cp src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

