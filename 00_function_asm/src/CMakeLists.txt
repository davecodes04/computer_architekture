add_executable(00_function_asm main.S my_strlen.S)

target_link_libraries(00_function_asm pico_stdlib)

pico_add_extra_outputs(00_function_asm)
