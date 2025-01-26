# libopencm3

set(LIBOPENCM3_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/libopencm3)
add_custom_target(libopencm3 make -j1 AR=${CMAKE_AR} CC=${CMAKE_C_COMPILER} lib/stm32/f4 WORKING_DIRECTORY ${LIBOPENCM3_DIR})

# platform

set(platform_dependencies libopencm3 PARENT_SCOPE)
set(platform_compiler_flags "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb-interwork -funroll-loops -fshort-enums" PARENT_SCOPE)
set(platform_linker_flags "-nostartfiles --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections" PARENT_SCOPE)

set(platform_sources
    # os
    ${CMAKE_CURRENT_SOURCE_DIR}/os/board.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os/os.cpp
    # drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Adc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ButtonLedMatrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ClockTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Console.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Dac.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Dio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GateOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/HighResolutionTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Lcd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Midi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/SdCard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ShiftRegister.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/System.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/UsbH.cpp
    # FreeRTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/kernel/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/kernel/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/kernel/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/kernel/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/kernel/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/kernel/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/portable/port.c
    # libusbhost
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libusbhost/src/usbh_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libusbhost/src/usbh_driver_ac_midi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libusbhost/src/usbh_driver_hub.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libusbhost/src/usbh_lld_stm32f4.c
    PARENT_SCOPE
)

set(platform_include_directories
    ${LIBOPENCM3_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/freertos/portable
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libusbhost/include
    PARENT_SCOPE
)

set(platform_libraries
    opencm3_stm32f4
    PARENT_SCOPE
)

set(platform_link_directories
    ${LIBOPENCM3_DIR}/lib
    PARENT_SCOPE
)

set(platform_defines
    -D PLATFORM_STM32
    -D STM32F4
    -D STM32F405xx
    PARENT_SCOPE
)

# toolchain

find_program(LINKER arm-none-eabi-ld)
find_program(OBJCOPY arm-none-eabi-objcopy)
find_program(OBJDUMP arm-none-eabi-objdump)
find_program(SIZE arm-none-eabi-size)
find_program(GDB arm-none-eabi-gdb)
find_program(OPENOCD openocd)
find_program(KERMIT ckermit)

# OpenOCD configuration

set(OPENOCD_INTERFACE ftdi/olimex-arm-usb-ocd-h)
set(OPENOCD_TARGET stm32f4x)
set(OPENOCD_INTERFACE ftdi/olimex-arm-usb-ocd-h PARENT_SCOPE)
set(OPENOCD_TARGET stm32f4x PARENT_SCOPE)

# functions

function(add_openocd_targets executable)
    add_custom_target(flash_${executable} DEPENDS ${executable} COMMAND ${OPENOCD}
        -f interface/${OPENOCD_INTERFACE}.cfg
        -f target/${OPENOCD_TARGET}.cfg
        -c "program ${executable} verify reset exit"
        ${NULL}
    )
endfunction(add_openocd_targets)

function(platform_postprocess_executable executable)
    set_target_properties(${executable} PROPERTIES LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/src/platform/stm32/stm32f405.ld")

    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${OBJCOPY} -Obinary ${executable} ${executable}.bin)
    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${OBJCOPY} -Oihex ${executable} ${executable}.hex)
    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${OBJCOPY} -Osrec ${executable} ${executable}.srec)
    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${OBJDUMP} -dhS ${executable} > ${executable}.list)
    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${SIZE} -A ${executable} > ${executable}.size)
    # TODO: Generate a map file when initially linking the exe.
    # Newer linker complains about using an executable as input, so this is disabled
    #    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${LINKER} -Map ${executable}.map ${executable} -o ${executable}.tmp && rm ${executable}.tmp)
    add_custom_command(TARGET ${executable} POST_BUILD COMMAND ${SIZE} --format=berkeley ${executable})
    add_openocd_targets(${executable})
endfunction(platform_postprocess_executable)

add_custom_target(openocd COMMAND ${OPENOCD} -f interface/${OPENOCD_INTERFACE}.cfg -f target/${OPENOCD_TARGET}.cfg)
