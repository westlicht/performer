#!/bin/sh

# stm32
(mkdir -p build/stm32/debug && cd build/stm32/debug && cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=stm32 ../../..)
(mkdir -p build/stm32/release && cd build/stm32/release && cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=stm32 ../../..)

# sim
(mkdir -p build/sim/debug && cd build/sim/debug && cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=sim ../../..)
(mkdir -p build/sim/release && cd build/sim/release && cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=sim ../../..)
(mkdir -p build/sim/www && cd build/sim/www && cmake -DCMAKE_TOOLCHAIN_FILE="${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_BUILD_TYPE=Release -DPLATFORM=sim ../../..)
