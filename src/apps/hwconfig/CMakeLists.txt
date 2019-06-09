if(${PLATFORM} STREQUAL "stm32")

    function(create_config name reverse_encoder invert_leds)
        add_executable(hwconfig_${name} hwconfig.c)
        target_compile_definitions(hwconfig_${name} PRIVATE "REVERSE_ENCODER=${reverse_encoder}")
        target_compile_definitions(hwconfig_${name} PRIVATE "INVERT_LEDS=${invert_leds}")
        platform_postprocess_executable(hwconfig_${name})
        # override linker script
        set_target_properties(hwconfig_${name} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/hwconfig.ld")
    endfunction(create_config)

    create_config(default 0 0)
    create_config(reverse_encoder 1 0)
    create_config(invert_leds 0 1)
    create_config(reverse_encoder_invert_leds 1 1)

endif()
