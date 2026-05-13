add_executable(${PROJECT_NAME} button.c)

target_link_libraries(${PROJECT_NAME} pico_runtime hardware_gpio)

pico_add_extra_outputs(${PROJECT_NAME})
