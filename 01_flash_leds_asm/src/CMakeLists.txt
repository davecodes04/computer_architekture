add_executable(01_flash_all_leds_asm main.S led.S my_sleep_ms.S)

target_link_libraries(01_flash_all_leds_asm pico_stdlib)

pico_add_extra_outputs(01_flash_all_leds_asm)
