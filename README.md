Build Status: ![Build Status](https://github.com/jackpf/performer/actions/workflows/ci.yml/badge.svg?branch=master)
Upstream Build Status: ![Upstream Build Status](https://github.com/westlicht/performer/actions/workflows/ci.yml/badge.svg?branch=master)

# Improvements

This is a fork of the [original repository](https://github.com/westlicht/performer), with some improvements that I personally find essential.

To find out more about improvements changes, check the table below.

| Change             	| Documentation                                    	|
|--------------------	|--------------------------------------------------	|
| Noise reduction    	| [docs](./doc/improvements/noise-reduction.md)    	|
| Shape improvements 	| [docs](./doc/improvements/shape-improvements.md) 	|
| MIDI improvements 	| [docs](./doc/improvements/midi-improvements.md) 	|

--- original documentation below ---

# PER|FORMER

<a href="doc/sequencer.jpg"><img src="doc/sequencer.jpg"/></a>

## Overview

This repository contains the firmware for the **PER|FORMER** eurorack sequencer.

For more information on the project go [here](https://westlicht.github.io/performer).

The hardware design files are hosted in a separate repository [here](https://github.com/westlicht/performer-hardware).

## Development

If you want to do development on the firmware, the following is a quick guide on how to setup the development environment to get you going.

### Setup on macOS and Linux

First you have to clone this repository (make sure to add the `--recursive` option to also clone all the submodules):

```
git clone --recursive https://github.com/westlicht/performer.git
```

After cloning, enter the performer directory:

```
cd performer
```

Make sure you have a recent version of CMake installed. If you are on Linux, you might also want to install a few other packages. For Debian based systems, use:

```
sudo apt-get install libtool autoconf cmake libusb-1.0.0-dev libftdi-dev pkg-config
```

To compile for the hardware and allow flashing firmware you have to install the ARM toolchain and build OpenOCD:

```
make tools_install
```

Next, you have to setup the build directories:

```
make setup_stm32
```

If you also want to compile/run the simulator use:

```
make setup_sim
```

The simulator is great when developing new features. It allows for a faster development cycle and a better debugging experience.

### Setup on Windows

Currently, there is no native support for compiling the firmware on Windows. As a workaround, there is a Vagrantfile to allow setting up a Vagrant virtual machine running Linux for compiling the application.

First you have to clone this repository (make sure to add the `--recursive` option to also clone all the submodules):

```
git clone --recursive https://github.com/westlicht/performer.git
```

Next, go to https://www.vagrantup.com/downloads.html and download the latest Vagrant release. Once installed, use the following to setup the Vagrant machine:

```
cd performer
vagrant up
```

This will take a while. When finished, you have a virtual machine ready to go. To open a shell, use the following:

```
vagrant ssh
```

When logged in, you can follow the development instructions below, everything is now just the same as with a native development environment on macOS or Linux. The only difference is that while you have access to all the source code on your local machine, you use the virtual machine for compiling the source.

To stop the virtual machine, log out of the shell and use:

```
vagrant halt
```

You can also remove the virtual machine using:

```
vagrant destroy
```

### Build directories

After successfully setting up the development environment you should now have a list of build directories found under `build/[stm32|sim]/[release|debug]`. The `release` targets are used for compiling releases (more code optimization, smaller binaries) whereas the `debug` targets are used for compiling debug releases (less code optimization, larger binaries, better debugging support).

### Developing for the hardware

You will typically use the `release` target when building for the hardware. So you first have to enter the release build directory:

```
cd build/stm32/release
```

To compile everything, simply use:

```
make -j
```

Using the `-j` option is generally a good idea as it enables parallel building for faster build times.

To compile individual applications, use the following make targets:

- `make -j sequencer` - Main sequencer application
- `make -j sequencer_standalone` - Main sequencer application running without bootloader
- `make -j bootloader` - Bootloader
- `make -j tester` - Hardware tester application
- `make -j tester_standalone` - Hardware tester application running without bootloader

Building a target generates a list of files. For example, after building the sequencer application you should find the following files in the `src/apps/sequencer` directory relative to the build directory:

- `sequencer` - ELF binary (containing debug symbols)
- `sequencer.bin` - Raw binary
- `sequencer.hex` - Intel HEX file (for flashing)
- `sequencer.srec` - Motorola SREC file (for flashing)
- `sequencer.list` - List file containing full disassembly
- `sequencer.map` - Map file containing section/offset information of each symbol
- `sequencer.size` - Size file containing size of each section

If compiling the sequencer, an additional `UPDATE.DAT` file is generated which can be used for flashing the firmware using the bootloader.

To simplify flashing an application to the hardware during development, each application has an associated `flash` target. For example, to flash the bootloader followed by the sequencer application use:

```
make -j flash_bootloader
make -j flash_sequencer
```

Flashing to the hardware is done using OpenOCD. By default, this expects an Olimex ARM-USB-OCD-H JTAG to be attached to the USB port. You can easily reconfigure this to use a different JTAG by editing the `OPENOCD_INTERFACE` variable in the `src/platform/stm32/CMakeLists.txt` file. Make sure to change both occurrences. A list of available interfaces can be found in the `tools/openocd/share/openocd/scripts/interface` directory (or `/home/vagrant/tools/openocd/share/openocd/scripts/interface` when running the virtual machine).

### Developing for the simulator

Note that the simulator is only supported on macOS and Linux and does not currently run in the virtual machine required on Windows.

You will typically use the `debug` target when building for the simulator. So you first have to enter the debug build directory:

```
cd build/sim/debug
```

To compile everything, simply use:

```
make -j
```

To run the simulator, use the following:

```
./src/apps/sequencer/sequencer
```

Note that you have to start the simulator from the build directory in order for it to find all the assets.

### Source code directory structure

The following is a quick overview of the source code directory structure:

- `src` - Top level source directory
- `src/apps` - Applications
- `src/apps/bootloader` - Bootloader application
- `src/apps/hwconfig` - Hardware configuration files
- `src/apps/sequencer` - Main sequencer application
- `src/apps/tester` - Hardware tester application
- `src/core` - Core library used by both the sequencer and hardware tester application
- `src/libs` - Third party libraries
- `src/os` - Shared OS helpers
- `src/platform` - Platform abstractions
- `src/platform/sim` - Simulator platform
- `src/platform/stm32` - STM32 platform
- `src/test` - Test infrastructure
- `src/tests` - Unit and integration tests

The two platforms both have a common subdirectories:

- `drivers` - Device drivers
- `libs` - Third party libraries
- `os` - OS abstraction layer
- `test` - Test runners

The main sequencer application has the following structure:

- `asteroids` - Asteroids game
- `engine` - Engine responsible for running the sequencer core
- `model` - Data model storing the live state of the sequencer and many methods to change that state
- `python` - Python bindings for running tests using python
- `tests` - Python based tests
- `ui` - User interface

## Third Party Libraries

The following third party libraries are used in this project.

- [FreeRTOS](http://www.freertos.org)
- [libopencm3](https://github.com/libopencm3/libopencm3)
- [libusbhost](https://github.com/libusbhost/libusbhost)
- [NanoVG](https://github.com/memononen/nanovg)
- [FatFs](http://elm-chan.org/fsw/ff/00index_e.html)
- [stb_sprintf](https://github.com/nothings/stb/blob/master/stb_sprintf.h)
- [stb_image_write](https://github.com/nothings/stb/blob/master/stb_image_write.h)
- [soloud](https://sol.gfxile.net/soloud/)
- [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/)
- [pybind11](https://github.com/pybind/pybind11)
- [tinyformat](https://github.com/c42f/tinyformat)
- [args](https://github.com/Taywee/args)

## License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This work is licensed under a [MIT License](https://opensource.org/licenses/MIT).
