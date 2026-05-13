cmake_minimum_required(VERSION 3.25)

include(${CMAKE_CURRENT_LIST_DIR}/../config.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../external/pico-sdk/pico_sdk_init.cmake)

project(lab2a)

pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/Config ${CMAKE_CURRENT_BINARY_DIR}/lib/Config)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/Fonts ${CMAKE_CURRENT_BINARY_DIR}/lib/Fonts)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/gui ${CMAKE_CURRENT_BINARY_DIR}/lib/gui)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/lcd ${CMAKE_CURRENT_BINARY_DIR}/lib/lcd)
add_subdirectory(src)
