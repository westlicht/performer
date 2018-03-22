
# SDL2

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(SDL_FLAGS "-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s USE_SDL_TTF=2 -s WASM=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" PARENT_SCOPE)
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    set(SDL_FLAGS "")
endif()

# RtMidi

add_subdirectory(libs/rtmidi)

# platform

set(platform_dependencies "" PARENT_SCOPE)
set(platform_compiler_flags "-Wno-unknown-pragmas -Wno-unknown-attributes ${SDL_FLAGS}" PARENT_SCOPE)
set(platform_linker_flags "" PARENT_SCOPE)

set(platform_sources
    # soloud
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/audiosource/wav/soloud_wav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/audiosource/wav/stb_vorbis.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/backend/sdl2_static/soloud_sdl2_static.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_audiosource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_3d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_basicops.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_faderops.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_filterops.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_getters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_setters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_voicegroup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_core_voiceops.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_fader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_fft_lut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_fft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_filter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud_thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/src/core/soloud.cpp
    # stb
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb/stb_image_write.cpp
    # drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ButtonLedMatrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Console.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/DIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Encoder.cpp
    # sim
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/Audio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/Midi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/Simulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/Text.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/instruments/DrumSampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/instruments/Synth.cpp

    PARENT_SCOPE
)

set(platform_include_directories
    ${SDL2_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}/..
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sdlpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/eigen
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/rtmidi
    PARENT_SCOPE
)

set(platform_libraries
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    rtmidi
    PARENT_SCOPE
)

set(platform_link_directories
    PARENT_SCOPE
)

set(platform_defines
    -D PLATFORM_SIM
    -D WITH_SDL2_STATIC
    PARENT_SCOPE
)

# functions

function(platform_postprocess_executable executable)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        # nothing for now
    endif()
endfunction(platform_postprocess_executable)
