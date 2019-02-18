cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/platform/stm32/libs/libopencm3/README.md)
message(STATUS "" )
message(STATUS "------------------------------------------------------------" )
message(STATUS "" )
message(STATUS "             !!! submodules not found !!!" )
message(STATUS "" )
message(STATUS "  did you forget to run 'git submodule update --init' ???" )
message(STATUS "------------------------------------------------------------" )
message(STATUS "" )
message(FATAL_ERROR "libopencm3 submodule not found!")
endif()

project(sequencer)
enable_testing()
add_subdirectory(src)
