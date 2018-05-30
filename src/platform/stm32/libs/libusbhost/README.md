[![Build Status](https://travis-ci.org/libusbhost/libusbhost.svg?branch=master)](https://travis-ci.org/libusbhost/libusbhost)
##General Information

[Link to the official repository](http://github.com/libusbhost/libusbhost)

###Objectives

- provide open-source(Lesser GPL3) usb host library for embedded devices
- execution speed. This library doesn't use blocking sleep,
making low overhead on runtime performance
- use static allocation for all of its buffers.
This means no allocation and reallocation is affecting performance
(possibility of memory fragmentation. execution time indeterminism). No malloc(), realloc(), free()
- do not depend on any operating system

### Supported hardware

- stm32f4discovery

### Supported device drivers

- HUB
- Gamepad - XBox compatible Controller
- Generic Human Interface driver: mouse, keyboard (raw data)
- USB MIDI devices (raw data + note on/off)

## Steps to compile library and demo
### Prerequisities
Make sure the following prerequisities are installed to be able to compile this library
- **git** for libopencm3 submodule fetch
- **gcc-arm-none-eabi** toolchain for cross compilation
- **cmake**
- **ccmake** (optional)
- **openocd** (optional)

### Basic setup
1. go to build directory located in the root of the project
> cd build

2. compile demo and the library with the default options set
> cmake .. && make

Executable demo is placed into `build/demo.hex`.
Library is placed into `build/src/libusbhost.a`.

### Advanced setup
*cmake* initial cache variables
<table>
<tr>
	<th>Cache variable</th><th>Value</th><th>Description</th>
</tr>
<tr>
	<td>USE_STM32F4_FS</td><td>TRUE</td><td>Enable STM32F4 Full Speed USB host peripheral</td>
</tr>
<tr>
	<td>USE_STM32F4_HS</td><td>TRUE</td><td>Enable STM32F4 High Speed USB host peripheral</td>
</tr>
<tr>
	<td>USE_USART_DEBUG</td><td>TRUE</td><td>Enable writing of the debug information to USART6</td>
</tr>
<tr>
	<td>OOCD_INTERFACE</td><td>"stlink-v2"</td><td>Interface configuration file used by the openocd</td>
</tr>
<tr>
	<td>OOCD_BOARD</td><td>"stm32f4discovery"</td><td>Board configuration file used by the openocd</td>
</tr>
</table>
You can alter these by issuing the following commands in the build directory

- Graphical user interface
> ccmake ..

- Command line interface
> cmake .. -D{VARIABLE}={VALUE}

### Flashing
If the *openocd* is installed, `make flash` executed in the build directory
flashes the `build/demo.hex` to the stm32f4discovery board.

### Reading debug output
The following table represents the configuration of the debug output
<table>
<tr>
	<th>GPIO</th><td>GPIOC6</td>
</tr>
<tr>
	<th>USART periphery</th><td>USART6</td>
</tr>
<tr>
	<th>Function</th><td>UART TX</td>
</tr>
<tr>
	<th>Baud rate</th><td>921600</td>
</tr>
<tr>
	<th>Uart mode</th><td>8N1</td>
</tr>
</table>

## License

The libusbhost code is released under the terms of the GNU Lesser General
Public License (LGPL), version 3 or later.

See COPYING.GPL3 and COPYING.LGPL3 for details.
