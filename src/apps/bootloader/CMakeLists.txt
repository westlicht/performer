set(sources
    Bootloader.cpp
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

if(${PLATFORM} STREQUAL "stm32")
    add_executable(bootloader ${sources})
    target_include_directories(bootloader PRIVATE .)
    target_link_libraries(bootloader PUBLIC platform)
    target_linker_script(bootloader bootloader.ld)
    target_compile_options(bootloader PRIVATE "-Os")
    platform_postprocess_executable(bootloader)
endif()
