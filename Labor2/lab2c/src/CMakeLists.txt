add_executable(${PROJECT_NAME} clock.c clock_cursor.c clock_gui.c clock_time.c)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../lib)
target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib hardware_adc hardware_gpio Config lcd gui hardware_ticks)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

# Additionally enable the USB port for stdio (printf) use serial console to access
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
