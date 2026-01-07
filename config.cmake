# See https://github.com/raspberrypi/pico-sdk.git

set(PICO_PLATFORM "rp2350-riscv") # or "rp2350", which defaults to "rp2350-arm-s" -- we want RISC-V
set(PICO_BOARD "waveshare_rp2350_lcd_1.28") # or "waveshare_rp2040_lcd_1.28"

# set(picotool_DIR "${CMAKE_CURRENT_LIST_DIR}../external/usr/picotool")
set(PICOTOOL_FETCH_FROM_GIT_PATH "${CMAKE_CURRENT_LIST_DIR}/../external/picotool")

# To include a new LVGL-version, unpack in lib/, create a soft-link named lvgl and
# create lv_conf.h in lvgl/ by copying from lv_conf_template.h
# Make sure to set:
#  #define LV_COLOR_DEPTH 16
#  #define LV_COLOR_16_SWAP 1

message (STATUS "Setting LV_CONFIG")
set(CACHE{LV_BUILD_CONF_PATH} TYPE PATH HELP "Where and under what name to find lv_conf.h" VALUE "${CMAKE_CURRENT_LIST_DIR}/lib/lvgl_lv_conf.h")
set(CACHE{LV_BUILD_SET_CONFIG_OPTS} TYPE BOOL HELP "Use all values from lv_conf.h" VALUE OFF)
set(CACHE{CONFIG_LV_BUILD_DEMOS} TYPE BOOL HELP "Disable Demos" VALUE OFF)
set(CACHE{CONFIG_LV_BUILD_EXAMPLES} TYPE BOOL HELP "Disable Examples" VALUE OFF)
set(CACHE{CONFIG_LV_USE_THORVG_INTERNAL} TYPE BOOL HELP "Disable ThorVG" VALUE OFF)

cmake_path(SET PATH "${PROJECT_SOURCE_DIR}../external/usr/bin:${PATH}")
