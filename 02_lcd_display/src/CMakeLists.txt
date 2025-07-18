add_executable(lcd_display lcd_display.c)

target_include_directories(lcd_display PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/)
target_link_libraries(lcd_display pico_stdlib lcd)

pico_add_extra_outputs(lcd_display)
