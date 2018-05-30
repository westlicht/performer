if(${PLATFORM} MATCHES "stm32")

    function(create_config variant)
        add_executable(hwconfig_${variant} hwconfig_${variant}.c)
        platform_postprocess_executable(hwconfig_${variant})
        # override linker script
        set_target_properties(hwconfig_${variant} PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/hwconfig.ld")
    endfunction(create_config)

    create_config(default)
    create_config(reversed_encoder)

endif()
