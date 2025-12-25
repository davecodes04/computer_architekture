# See https://github.com/raspberrypi/pico-sdk.git

set(PICO_PLATFORM "rp2350-riscv") # or "rp2350", which defaults to "rp2350-arm-s" -- we want RISC-V
set(PICO_BOARD "waveshare_rp2350_lcd_1.28") # or "waveshare_rp2040_lcd_1.28"

# set(picotool_DIR "${CMAKE_CURRENT_LIST_DIR}../external/usr/picotool")
set(PICOTOOL_FETCH_FROM_GIT_PATH "${CMAKE_CURRENT_LIST_DIR}/../external/picotool")

cmake_path(SET PATH "${PROJECT_SOURCE_DIR}../external/usr/bin:${PATH}")
