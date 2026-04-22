add_executable(${PROJECT_NAME} main.S led.S my_delay.S)

target_link_libraries(${PROJECT_NAME} pico_runtime)

pico_add_extra_outputs(${PROJECT_NAME})
