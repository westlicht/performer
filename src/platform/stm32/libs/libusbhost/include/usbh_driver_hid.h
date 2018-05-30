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

#ifndef USBH_DRIVER_HID_
#define USBH_DRIVER_HID_

#include "usbh_core.h"

#include <stdint.h>

BEGIN_DECLS

struct _hid_mouse_config {
	/**
	 * @brief this is called when some data is read when polling the device
	 * @param device_id handle of HID device
	 * @param data pointer to the data
	 * @param length count of bytes in the data
	 *
	 * TODO: make better interface that provides data contained in the report descriptor
	 *
	 */
	void (*hid_in_message_handler)(uint8_t device_id, const uint8_t *data, uint32_t length);
};
typedef struct _hid_mouse_config hid_config_t;

/**
 * @brief hid_mouse_driver_init initialization routine - this will initialize internal structures of this device driver
 * @param config
 * @see hid_mouse_config_t
 */
void hid_driver_init(const hid_config_t *config);

/**
 * @brief hid_set_report
 * @param device_id handle of HID device
 * @returns true on success, false otherwise
 */
bool hid_set_report(uint8_t device_id, uint8_t val);

enum HID_TYPE {
	HID_TYPE_NONE,
	HID_TYPE_MOUSE,
	HID_TYPE_KEYBOARD,
};

/**
 * @brief hid_get_type
 * @param device_id handle of HID device
 * @return type of attached HID
 * @see enum HID_TYPE
 */
enum HID_TYPE hid_get_type(uint8_t device_id);

/**
 * @brief hid_is_connected
 * @param device_id handle of HID device
 * @return true if the device with device_id is connected
 */
bool hid_is_connected(uint8_t device_id);

extern const usbh_dev_driver_t usbh_hid_driver;

END_DECLS

#endif
