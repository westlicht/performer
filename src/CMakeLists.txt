
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

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fdata-sections -ffunction-sections ${platform_compiler_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-function -fdata-sections -ffunction-sections ${platform_compiler_flags}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${platform_linker_flags}")

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

set(core_sources
    # stb
    libs/stb/stb_sprintf.c
    # ff
    libs/ff/ff.c
    # core
    core/Debug.cpp
    core/fs/Error.cpp
    core/fs/File.cpp
    core/fs/FileSystem.cpp
    core/fs/Volume.cpp
    core/gfx/Canvas.cpp
    core/math/Mat3.cpp
    core/math/Mat4.cpp
    core/math/Math.cpp
    core/math/Vec2.cpp
    core/math/Vec3.cpp
    core/math/Vec4.cpp
    core/midi/MidiMessage.cpp
    core/midi/MidiParser.cpp
    core/profiler/Profiler.cpp
)

include_directories(.)
include_directories(libs)

include_directories(${platform_include_directories})
link_directories(${platform_link_directories})

# for now platform and core are built together because core is also used by platform
add_library(core STATIC ${core_sources} ${platform_sources})
target_link_libraries(core ${platform_libraries})
if(NOT ${platform_dependencies} STREQUAL "")
add_dependencies(core ${platform_dependencies})
endif()


# applications
add_subdirectory(apps)

# tests
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    add_subdirectory(tests)
endif()
