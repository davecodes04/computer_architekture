add_library(lcd STATIC lcd.c)

target_link_libraries(lcd hardware_adc hardware_dma hardware_spi hardware_i2c hardware_pwm)

SET_TARGET_PROPERTIES(lcd 
    PROPERTIES 
       LINKER_LANGUAGE C
)