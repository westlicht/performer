/*
 * This file is part of the libusbhost library
 * hosted at http://github.com/libusbhost/libusbhost
 *
 * Copyright (C) 2016 Amir Hammad <amir.hammad@hotmail.com>
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

#ifndef USBH_DRIVER_AC_MIDI_PRIVATE_
#define USBH_DRIVER_AC_MIDI_PRIVATE_

#include "driver/usbh_device_driver.h"
#include "usbh_driver_ac_midi.h"

#include <stdint.h>


#define MIDI_INITIAL_DELAY 	(100000)

struct _midi_device {
	uint16_t vendor_id;
	uint16_t product_id;
	usbh_device_t *usbh_device;
	uint8_t buffer[USBH_AC_MIDI_BUFFER];
	uint16_t endpoint_in_maxpacketsize;
	uint16_t endpoint_out_maxpacketsize;
	uint8_t endpoint_in_address;
	uint8_t endpoint_out_address;
	uint8_t state;
	uint8_t endpoint_in_toggle;
	uint8_t endpoint_out_toggle;
	uint8_t device_id;
	bool sending;
	midi_write_callback_t write_callback_user;
	usbh_packet_t write_packet;
	// Timestamp at sending config command
	uint32_t time_us_config;
};
typedef struct _midi_device midi_device_t;
#endif
