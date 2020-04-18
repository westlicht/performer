if(${PLATFORM} STREQUAL "stm32")

    function(create_config name reverse_encoder invert_leds dac_type)
        add_executable(hwconfig_${name} hwconfig.c)
        target_compile_definitions(hwconfig_${name} PRIVATE "REVERSE_ENCODER=${reverse_encoder}")
        target_compile_definitions(hwconfig_${name} PRIVATE "INVERT_LEDS=${invert_leds}")
        target_compile_definitions(hwconfig_${name} PRIVATE "DAC_TYPE=${dac_type}")
        platform_postprocess_executable(hwconfig_${name})
        # override linker script
        set_target_properties(hwconfig_${name} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/hwconfig.ld")
    endfunction(create_config)

    create_config(default                               0 0 0)
    create_config(dac8568a                              0 0 1)
    create_config(invert-leds                           0 1 0)
    create_config(invert-leds_dac8568a                  0 1 1)
    create_config(reverse-encoder                       1 0 0)
    create_config(reverse-encoder_dac8568a              1 0 1)
    create_config(reverse-encoder_invert-leds           1 1 0)
    create_config(reverse-encoder_invert-leds_dac8568a  1 1 1)

endif()
