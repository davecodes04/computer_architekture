aux_source_directory(. DIR_LCD_SRCS)

add_library(lcd STATIC ${DIR_LCD_SRCS})
target_include_directories(lcd PUBLIC ../Config)
target_link_libraries(lcd Config pico_stdlib hardware_adc hardware_dma hardware_spi hardware_i2c hardware_pwm)

SET_TARGET_PROPERTIES(lcd 
    PROPERTIES 
       LINKER_LANGUAGE C
)
