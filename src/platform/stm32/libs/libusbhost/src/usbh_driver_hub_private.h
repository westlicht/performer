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

#ifndef USBH_DRIVER_HUB_PRIVATE_
#define USBH_DRIVER_HUB_PRIVATE_

#include "usbh_config.h"
#include "driver/usbh_device_driver.h"
#include "usbh_driver_hub.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/usb/usbstd.h>


// # HUB DEFINITIONS
#define HUB_FEATURE_PORT_CONNECTION 0
#define HUB_FEATURE_PORT_ENABLE 1
#define HUB_FEATURE_PORT_SUSPEND 2
#define HUB_FEATURE_PORT_OVERCURRENT 3
#define HUB_FEATURE_PORT_RESET 4
#define HUB_FEATURE_PORT_POWER 8
#define HUB_FEATURE_PORT_LOWSPEED 9
#define HUB_FEATURE_PORT_HIGHSPEED 10

#define HUB_FEATURE_C_PORT_CONNECTION 16
#define HUB_FEATURE_C_PORT_ENABLE 17
#define HUB_FEATURE_C_PORT_SUSPEND 18
#define HUB_FEATURE_C_PORT_OVERCURRENT 19
#define HUB_FEATURE_C_PORT_RESET 20

#define HUB_REQ_GET_STATUS 		0
#define HUB_REQ_CLEAR_FEATURE 	1
#define HUB_REQ_SET_FEATURE 	3
#define HUB_REQ_GET_DESCRIPTOR 	6

#define USB_DT_HUB 		(41)
#define USB_DT_HUB_SIZE	(9)
// Hub buffer: must be larger than hub descriptor
#define USBH_HUB_BUFFER_SIZE	(USB_DT_HUB_SIZE)


#define CURRENT_PORT_NONE -1

enum EVENT_STATE {
	EVENT_STATE_NONE,
	EVENT_STATE_INITIAL,
	EVENT_STATE_POLL_REQ,
	EVENT_STATE_POLL,
	EVENT_STATE_READ_HUB_DESCRIPTOR_COMPLETE,
	EVENT_STATE_ENABLE_PORTS,
	EVENT_STATE_GET_PORT_STATUS,
	EVENT_STATE_PORT_RESET_REQ,
	EVENT_STATE_PORT_RESET_COMPLETE,
	EVENT_STATE_SLEEP_500_MS,
	EVENT_STATE_GET_STATUS_COMPLETE,
};

struct _hub_device {
	usbh_device_t *device[USBH_HUB_MAX_DEVICES + 1];
	uint8_t buffer[USBH_HUB_BUFFER_SIZE];
	uint16_t endpoint_in_maxpacketsize;
	uint8_t endpoint_in_address;
	uint8_t endpoint_in_toggle;
	enum EVENT_STATE state;

	uint8_t desc_len;
	uint16_t ports_num;
	int8_t index;
	int8_t current_port;

	struct {
		uint16_t sts;
		uint16_t stc;
	} hub_and_port_status[USBH_HUB_MAX_DEVICES + 1];

	bool busy;

	uint32_t time_curr_us;
	uint32_t timestamp_us;
};

typedef struct _hub_device hub_device_t;

struct usb_hub_descriptor_head {
	uint8_t bDescLength;
	uint8_t bDescriptorType;
	uint8_t bNbrPorts;
	uint16_t wHubCharacteristics;
	uint8_t bPwrOn2PwrGood;
	uint8_t bHubContrCurrent;
} __attribute__((packed));
struct usb_hub_descriptor_body {
	uint8_t bDeviceRemovable;
	uint8_t PortPwrCtrlMask;
} __attribute__((packed));

// for hubs with up to 7 ports on hub
struct usb_hub_descriptor {
	struct usb_hub_descriptor_head head;
	struct usb_hub_descriptor_body body[1];
} __attribute__((packed));

#endif
