set(sources
    Canvas.cpp
    Console.cpp
    Encoder.cpp
    FileSystem.cpp
    Lcd.cpp
    MD5.cpp
    SdCard.cpp
    System.cpp
    UpdateFile.cpp
    # lib
    lib/stb_sprintf.c
    lib/ff/ff.c
)

include_directories(.)

if(${PLATFORM} STREQUAL "stm32")
    add_executable(bootloader Bootloader.cpp ${sources})
    target_link_libraries(bootloader ${platform_libraries})
    platform_postprocess_executable(bootloader)
    add_dependencies(bootloader ${platform_dependencies})
    # override linker script
    set_target_properties(bootloader PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/bootloader.ld")
    set_target_properties(bootloader PROPERTIES COMPILE_FLAGS "-Os")
endif()
