# About
Project to use the 1.28" Display showing the _proper_ usage of the LVGL library.
This has been tested with LVGL-9.4.0, uses proper timing functions with PICO-SDK,
unlike WaveShare example, which assumes a 5ms tick is always 5ms after the previous
disregarding the time LVGL's `lv_timer_handler()` uses.  The WaveShare examples's
technique is specifically discouraged in LVGL's documentation.

This examples uses a few of LVGL's widget to display 
 - a Table: RISC-V's executed number of instructions and clock cycles, demonstrating
   the usage of RISC-V's CSR to stop inhibiting these two hardware counters.
 - a scales simulating the IMU's -90° to +90° gyro data. Being animated, this
   results in many LVGL redraws, showing off Frames-per-second (FPS) using
   different C preprocessor options, as described below.

Regarding the DISPLAY BUFFER OPTIONS:
This example also demonstrates the usage of partial and full rendering:
  - Full Buffer rendering with 1, 2 or even 3 buffers:
    The (small) advantage is, that the display buffer to write data to does
    NOT need be be reset using LCD_1IN28_SetWindows()
    The disadvantage of this method is: it takes considerable amount of
    memory: 1 display buffer is 240 * 240 * 2 Bytes
    With double-buffering this is in total 230.400 Bytes,
    with triple-bufferig this is in total 345.600 Bytes (the RP2350 has 520 kB total RAM...)
  - 1/2 buffer rendering with 1, 2 or even 3 buffers allowing overlapping of rendering,
    DMA-transfer to be initiated and DMA-transfer (in flight).
    The advantage is, that with relatively low overhead (2 buffers) or only 50% overhead (3 buffers),
    there is considerable overlap!
    The disadvantage with partial rendering is: for every partial frame,
    the LCD driver needs to know the new display buffer to write data to
    using LCD_1IN28_SetWindows().
  - 1/3 and other DISPLAY_BUFFER_DIVISORS: buffer rendering with 1, 2 or even 3 buffers
    This only uses a fraction of the memory required otherwise for a full image.
    LVGL's documentation recommends at least 1/10th of the buffer size.
All of these modes display the frames-per-second (FPS) in the bottom as a label.
Please note: LVGL will only render a new screen, if there were updates (anything drawn, any animation going on)!
Even so, LVGL internally defines the constant `LV_DEF_REFR_PERIOD`, with a 
default display refresh, input device read and animation step period of 33ms.
So without changing this constant, we will not see more than 30 FPS (and we do not have to...).

This example is based on
 - the LVGL project
 - Pico-SDK
 - CMake

# Board Setup
Nothing special, there's no additional hardware on the breadboard.

# Compilation
Just do
  cmake -G Ninja -Bbuild .
  cmake --build build
  cp build/src/*.uf2 /Volumes/RP2350    # or wherever your RP2350 shows up
