
message(STATUS "Building for platform ${PLATFORM} ...")

set(CMAKE_CXX_STANDARD 11)

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

add_library(platform INTERFACE)

# separate_arguments(platform_compiler_flags)
# target_compile_options(platform INTERFACE ${platform_compiler_flags})
target_compile_definitions(platform INTERFACE ${platform_defines})
add_dependencies(platform ${platform_dependencies})
target_link_libraries(platform INTERFACE ${platform_libraries})
set_property(TARGET platform PROPERTY LINK_FLAGS ${platform_linker_flags})

# set_target_properties(platform PROPERTIES LINK_FLAGS ${platform_linker_flags})
# separate_arguments(platform_linker_flags)
# target_compile_options(platform INTERFACE ${platform_linker_flags})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${platform_linker_flags}")

# add_definitions(${platform_defines})


add_library(core STATIC)

target_link_libraries(core PUBLIC platform)

target_sources(core PRIVATE
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

target_sources(core PRIVATE ${platform_sources})


# compiler flags
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fdata-sections -ffunction-sections ${platform_compiler_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-function --std=c++11 -fdata-sections -ffunction-sections ${platform_compiler_flags}")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${platform_linker_flags}")

# target_compile_options(core INTERFACE -g -Wall -fdata-sections -ffunction-sections -Wno-unused-function)

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
# add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
target_compile_definitions(core PRIVATE "-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# set(core_sources
#     # stb
#     libs/stb/stb_sprintf.c
#     # ff
#     libs/ff/ff.c
#     # core
#     core/Debug.cpp
#     core/fs/Error.cpp
#     core/fs/File.cpp
#     core/fs/FileSystem.cpp
#     core/fs/Volume.cpp
#     core/gfx/Canvas.cpp
#     core/math/Mat3.cpp
#     core/math/Mat4.cpp
#     core/math/Math.cpp
#     core/math/Vec2.cpp
#     core/math/Vec3.cpp
#     core/math/Vec4.cpp
#     core/midi/MidiMessage.cpp
#     core/midi/MidiParser.cpp
#     core/profiler/Profiler.cpp
# )

include_directories(.)

target_include_directories(core PUBLIC libs)
# include_directories(libs)

include_directories(${platform_include_directories})
link_directories(${platform_link_directories})

# for now platform and core are built together because core is also used by platform
# add_library(core STATIC ${core_sources} ${platform_sources})
if(NOT ${platform_dependencies} STREQUAL "")
# add_dependencies(core ${platform_dependencies})
endif()


# applications
add_subdirectory(apps)

# tests
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    add_subdirectory(tests)
endif()
