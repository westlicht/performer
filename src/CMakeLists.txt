
message(STATUS "Building for platform ${PLATFORM} ...")

# include platform
# defines the following variables:
# - platform_dependencies
# - platform_compiler_flags
# - platform_linker_flags
# - platform_sources
# - platform_include_directories
# - platform_libraries
# - platform_link_directories
# - platform_defines
# defines the following functions:
# - platform_postprocess_executable
add_subdirectory(platform/${PLATFORM})

add_definitions(${platform_defines})

# compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fdata-sections -ffunction-sections ${platform_compiler_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fdata-sections -ffunction-sections ${platform_compiler_flags}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${platform_linker_flags}")

set(core_sources
    # stb
    libs/stb/stb_sprintf.c
    # core
    core/Debug.cpp
    core/gfx/Canvas.cpp
    core/midi/MIDIMessage.cpp
    core/midi/MIDIParser.cpp
    core/profiler/Profiler.cpp
)

set(sources
    # engine
    engine/Engine.cpp
    engine/Clock.cpp
    engine/Track.cpp
    # ui
    ui/LedPainter.cpp
    ui/Page.cpp
    ui/UI.cpp
    ui/pages/MainPage.cpp
    ui/pages/MutePage.cpp
    ui/pages/TopPage.cpp
    ui/pages/TrackPage.cpp
    ui/painters/WindowPainter.cpp
)

include_directories(.)
include_directories(libs)

include_directories(${platform_include_directories})
link_directories(${platform_link_directories})

add_library(platform STATIC ${platform_sources})
if(NOT ${platform_dependencies} STREQUAL "")
    add_dependencies(platform ${platform_dependencies})
endif()
target_link_libraries(platform ${platform_libraries})

add_library(core STATIC ${core_sources})
target_link_libraries(core platform)

if(${PLATFORM} MATCHES "stm32")
    add_executable(sequencer apps/sequencer/sequencer.cpp ${sources})
    target_link_libraries(sequencer core)
    platform_postprocess_executable(sequencer)
endif()

if(${PLATFORM} MATCHES "sim")
    add_executable(sequencer apps/sequencer/SequencerSim.cpp ${sources})
    target_link_libraries(sequencer core)
    platform_postprocess_executable(sequencer)
    add_custom_command(TARGET sequencer COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/platform/sim/assets assets)
endif()


# tests

function(register_test test file)
    add_executable(${test} ${file})
    target_link_libraries(${test} core)
    platform_postprocess_executable(${test})
endfunction(register_test)

add_subdirectory(tests)
