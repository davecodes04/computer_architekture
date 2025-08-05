add_executable(lcd_display lcd_display.c)

target_include_directories(lcd_display PUBLIC ../../lib)
target_link_libraries(lcd_display pico_stdlib Config lcd)

# Create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(lcd_display)

# Additionally enable the USB port for stdio (printf) use serial console to access
pico_enable_stdio_usb(lcd_display 1)
