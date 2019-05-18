
# SDL2

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(SDL_FLAGS "-s USE_SDL=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
    set(CMAKE_EXECUTABLE_SUFFIX ".html" PARENT_SCOPE)
else()
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    set(SDL_FLAGS "")
endif()

# RtMidi

add_subdirectory(libs/rtmidi)

# platform

set(platform_dependencies "" PARENT_SCOPE)
set(platform_compiler_flags "-fPIC -Wno-unknown-pragmas -Wno-unknown-attributes ${SDL_FLAGS}" PARENT_SCOPE)
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
    # nanovg
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/nanovg/src/nanovg.c
    # drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Console.cpp
    # sim
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/Simulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/TargetStateTracker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/TargetTrace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/TargetTracePlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/TargetTraceRecorder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/Audio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/Frontend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/InstrumentSetup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/Midi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/instruments/DrumSampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sim/frontend/instruments/Synth.cpp
    # os
    ${CMAKE_CURRENT_SOURCE_DIR}/os/os.cpp

    PARENT_SCOPE
)

set(platform_include_directories
    ${SDL2_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}/..
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/args
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/nanovg/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/rtmidi
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyformat
    PARENT_SCOPE
)

set(platform_libraries
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${OPENGL_LIBRARIES}
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
endfunction(platform_postprocess_executable)

# pybind11

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    add_subdirectory(libs/pybind11)
endif()
