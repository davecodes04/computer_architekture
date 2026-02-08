cmake_minimum_required(VERSION 3.25)

include(${CMAKE_CURRENT_LIST_DIR}/../config.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../external/pico-sdk/pico_sdk_init.cmake)

project(03_gpio_dcf77)

pico_sdk_init()

add_subdirectory(src)
