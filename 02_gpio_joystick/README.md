# About
This example shows how to use GPIO's ADC capability
to measure the position of the JoyStick.

It is based on
 - WaveShare's abstractions of Pico-SDK in lib/
 - Pico-SDK
 - CMake

# Board Setup
Please attach the provided JoyStick attached to the ADC pins.
Please check that GND is attached to the blue, VCC+ to the red
cable, the green should connect the button, while the grey
cables connect the potentiometer of X and Y to GPIO 26 and GPIO 27
respectively.
The joystick therefore faces outward, away from the Breadboard.

Make sure, You connect the ADC V_reference pin (north of GPIO 28)
to the Vcc + line, otherwise You may get garbage value when
reaching the positive X or positive Y direction.

# Compilation
Just do
  cmake -G Ninja -BBUILD .
  cmake --build BUILD
  cp src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up

