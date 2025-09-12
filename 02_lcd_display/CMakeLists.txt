cmake_minimum_required(VERSION 3.25)

include(${CMAKE_CURRENT_LIST_DIR}/../config.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../external/pico-sdk/pico_sdk_init.cmake)

project(02_lcd_display LANGUAGES C CXX)

pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/ ${CMAKE_CURRENT_BINARY_DIR}/lib)
add_subdirectory(src)
