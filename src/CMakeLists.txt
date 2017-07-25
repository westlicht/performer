
# include platform
# defines the following variables:
# - platform_compiler_flags
# - platform_linker_flags
# - platform_sources
# - platform_include_directories
# - platform_libraries
# - platform_link_directories
# - platform_defines
# defines the following functions:
# - platform_postprocess_executable
add_subdirectory(platform/stm32)

add_definitions(${platform_defines})

# compiler flags
set(TINYPRINTF_FLAGS "-Wl,--wrap,printf -Wl,--wrap,sprintf -Wl,--wrap,snprintf -Wl,--wrap,puts -Wl,--wrap,putchar")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fdata-sections -ffunction-sections ${platform_compiler_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall --std=c++11 -fdata-sections -ffunction-sections -fshort-enums ${platform_compiler_flags}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TINYPRINTF_FLAGS} -Wl,--gc-sections ${platform_linker_flags}")

set(sources
	# tinyprintf
	libs/tinyprintf/printf-c.cc
	# core
	core/gfx/Canvas.cpp
	core/profiler/Profiler.cpp
	# engine
	engine/Engine.cpp
	engine/Clock.cpp
	# ui
	ui/UI.cpp
)

include_directories(${platform_include_directories})
link_directories(${platform_link_directories})

include_directories(.)
include_directories(libs)

add_executable(sequencer apps/sequencer/sequencer.cpp ${sources} ${platform_sources})
target_link_libraries(sequencer ${platform_libraries})
add_dependencies(sequencer libopencm3)
platform_postprocess_executable(sequencer)
