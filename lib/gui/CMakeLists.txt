aux_source_directory(. DIR_GUI_SRCS)

add_library(gui STATIC ${DIR_GUI_SRCS})
target_include_directories(gui PUBLIC ../Config ../Fonts)
target_link_libraries(gui Config Fonts pico_stdlib hardware_adc hardware_dma hardware_spi hardware_i2c hardware_pwm)

SET_TARGET_PROPERTIES(gui
    PROPERTIES 
       LINKER_LANGUAGE C
)
