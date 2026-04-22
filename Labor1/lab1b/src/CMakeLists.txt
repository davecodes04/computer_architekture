# Add any Assembler files to add_executable
# add_executable(${PROJECT_NAME} main.S)
add_executable(${PROJECT_NAME} main.S decToASCII.S hexToASCII.S stringfct.S)

target_link_libraries(${PROJECT_NAME} pico_runtime)

pico_add_extra_outputs(${PROJECT_NAME})