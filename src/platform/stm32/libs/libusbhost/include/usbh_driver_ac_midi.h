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

#ifndef USBH_DRIVER_AC_MIDI_
#define USBH_DRIVER_AC_MIDI_

#include "usbh_core.h"

#include <stdint.h>

BEGIN_DECLS

struct _midi_config {
	void (*read_callback)(int device_id, uint8_t *data);
	void (*notify_connected)(int device_id, uint16_t vendor_id, uint16_t product_id, uint32_t max_packet_size);
	void (*notify_disconnected)(int device_id);
};
typedef struct _midi_config midi_config_t;

/**
 * @param bytes_written count of bytes that were actually written
 */
typedef void (*midi_write_callback_t)(uint8_t bytes_written);

/**
 * @brief midi_driver_init initialization routine - this will initialize internal structures of this device driver
 * @param config
 *
 * @see midi_config_t
 */
void midi_driver_init(const midi_config_t *config);

/**
 * @brief usbh_midi_write
 * @param device_id
 * @param data
 * @param length
 * @param callback this is called when the write call finishes
 */
void usbh_midi_write(uint8_t device_id, const void *data, uint32_t length, midi_write_callback_t callback);

extern const usbh_dev_driver_t usbh_midi_driver;

END_DECLS

#endif
