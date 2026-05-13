add_executable(${PROJECT_NAME} joystick_rgb.c)

target_link_libraries(${PROJECT_NAME} pico_runtime pico_time hardware_adc hardware_gpio hardware_pwm)

pico_add_extra_outputs(${PROJECT_NAME})
