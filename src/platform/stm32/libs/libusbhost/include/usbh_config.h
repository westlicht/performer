/*
 * This file is part of the libusbhost library
 * hosted at http://github.com/libusbhost/libusbhost
 *
 * Copyright (C) 2015 Amir Hammad <amir.hammad@hotmail.com>
 *
 *
 * libusbhost is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef USBH_CONFIG_
#define USBH_CONFIG_

#include "SystemConfig.h"

#if CONFIG_ENABLE_USBH_DEBUG
# define USART_DEBUG 1
#endif

#define USE_STM32F4_USBH_DRIVER_FS 1

// Max devices per hub
#define USBH_HUB_MAX_DEVICES	(4)

// Max number of hub instancies
#define USBH_MAX_HUBS		(1)

// Max devices
#define USBH_MAX_DEVICES		(15)

// Min: 128
// Set this wisely
#define BUFFER_ONE_BYTES	(2048)

// HID class devices
#define USBH_HID_MAX_DEVICES	(2)
#define USBH_HID_BUFFER		(256)
#define USBH_HID_REPORT_BUFFER (4)

// MIDI
// Maximal number of midi devices connected to whatever hub
#define USBH_AC_MIDI_MAX_DEVICES	(4)

#define USBH_AC_MIDI_BUFFER 	(64)

// Gamepad XBOX
#define USBH_GP_XBOX_MAX_DEVICES	(2)

#define USBH_GP_XBOX_BUFFER		(32)

/* Sanity checks */
#if (USBH_MAX_DEVICES > 127)
#error USBH_MAX_DEVICES > 127
#endif

#endif
