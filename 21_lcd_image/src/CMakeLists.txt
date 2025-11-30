add_executable(${CMAKE_PROJECT_NAME} lcd_image.c)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ../../lib)
target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib Config lcd)

# Create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

# Additionally enable the USB port for stdio (printf) use serial console to access
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
