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

#include "usbh_core.h"
#include "driver/usbh_device_driver.h"
#include "usbh_driver_hid.h"
#include "usart_helpers.h"

#include <libopencm3/usb/usbstd.h>
#include <libopencm3/usb/hid.h>
#include <stdint.h>
#include <stddef.h>

#define USB_HID_SET_REPORT 0x09
#define USB_HID_SET_IDLE 0x0A

enum STATES {
	STATE_INACTIVE,
	STATE_READING_REQUEST,
	STATE_READING_COMPLETE_AND_CHECK_REPORT,
	STATE_SET_REPORT_EMPTY_READ,
	STATE_GET_REPORT_DESCRIPTOR_READ_SETUP,// configuration is complete at this point. We write request
	STATE_GET_REPORT_DESCRIPTOR_READ_COMPLETE,// after the read finishes, we parse that descriptor
	STATE_SET_IDLE,
	STATE_SET_IDLE_COMPLETE,
};

enum REPORT_STATE {
	REPORT_STATE_NULL,
	REPORT_STATE_READY,
	REPORT_STATE_PENDING,
};

struct _hid_device {
	usbh_device_t *usbh_device;
	uint8_t buffer[USBH_HID_BUFFER];
	uint16_t endpoint_in_maxpacketsize;
	uint8_t endpoint_in_address;
	enum STATES state_next;
	uint8_t endpoint_in_toggle;
	uint8_t device_id;
	uint8_t configuration_value;
	uint16_t report0_length;
	enum REPORT_STATE report_state;
	uint8_t report_data[USBH_HID_REPORT_BUFFER];
	uint8_t report_data_length;
	enum HID_TYPE hid_type;
	uint8_t interface_number;
};
typedef struct _hid_device hid_device_t;

struct hid_report_decriptor {
	struct usb_hid_descriptor header;
	struct _report_descriptor_info {
		uint8_t bDescriptorType;
		uint16_t wDescriptorLength;
	} __attribute__((packed)) report_descriptors_info[];
} __attribute__((packed));

static hid_device_t hid_device[USBH_HID_MAX_DEVICES];
static hid_config_t hid_config;

static bool initialized = false;

void hid_driver_init(const hid_config_t *config)
{
	uint32_t i;

	initialized = true;

	hid_config = *config;
	for (i = 0; i < USBH_HID_MAX_DEVICES; i++) {
		hid_device[i].state_next = STATE_INACTIVE;
	}
}

static void *init(usbh_device_t *usbh_dev, const usbh_dev_driver_info_t * device_info)
{
	if (!initialized) {
		LOG_PRINTF("\n%s/%d : driver not initialized\r\n", __FILE__, __LINE__);
		return 0;
	}

	uint32_t i;
	hid_device_t *drvdata = NULL;

	// find free data space for HID device
	for (i = 0; i < USBH_HID_MAX_DEVICES; i++) {
		if (hid_device[i].state_next == STATE_INACTIVE) {
			drvdata = &hid_device[i];
			drvdata->vendorId = device_info->vendorId;
			drvdata->productId = device_info->productId;
			drvdata->device_id = i;
			drvdata->endpoint_in_address = 0;
			drvdata->endpoint_in_toggle = 0;
			drvdata->report0_length = 0;
			drvdata->usbh_device = usbh_dev;
			drvdata->report_state = REPORT_STATE_NULL;
			drvdata->hid_type = HID_TYPE_NONE;
			break;
		}
	}

	return drvdata;
}

static void parse_report_descriptor(hid_device_t *hid, const uint8_t *buffer, uint32_t length)
{
	// TODO
	// Do some parsing!
	// add some checks
	hid->report_state = REPORT_STATE_READY;

	// TODO: parse this from buffer!
	hid->report_data_length = 1;
	(void)buffer;
	(void)length;
}

/**
 * Returns true if all needed data are parsed
 */
static bool analyze_descriptor(void *drvdata, void *descriptor)
{
	hid_device_t *hid = (hid_device_t *)drvdata;
	uint8_t desc_type = ((uint8_t *)descriptor)[1];
	switch (desc_type) {
	case USB_DT_CONFIGURATION:
		{
			const struct usb_config_descriptor * cfg = (const struct usb_config_descriptor*)descriptor;
			hid->configuration_value = cfg->bConfigurationValue;
		}
		break;

	case USB_DT_DEVICE:
		{
			const struct usb_device_descriptor *devDesc = (const struct usb_device_descriptor *)descriptor;
			(void)devDesc;
		}
		break;

	case USB_DT_INTERFACE:
		{
			const struct usb_interface_descriptor *ifDesc = (const struct usb_interface_descriptor *)descriptor;
			if (ifDesc->bInterfaceProtocol == 0x01) {
				hid->hid_type = HID_TYPE_KEYBOARD;
				hid->interface_number = ifDesc->bInterfaceNumber;
			} else if (ifDesc->bInterfaceProtocol == 0x02) {
				hid->hid_type = HID_TYPE_MOUSE;
				hid->interface_number = ifDesc->bInterfaceNumber;
			}
		}
		break;

	case USB_DT_ENDPOINT:
		{
			const struct usb_endpoint_descriptor *ep = (const struct usb_endpoint_descriptor *)descriptor;
			if ((ep->bmAttributes&0x03) == USB_ENDPOINT_ATTR_INTERRUPT) {
				uint8_t epaddr = ep->bEndpointAddress;
				if (epaddr & (1<<7)) {
					hid->endpoint_in_address = epaddr&0x7f;
					if (ep->wMaxPacketSize < USBH_HID_BUFFER) {
						hid->endpoint_in_maxpacketsize = ep->wMaxPacketSize;
					} else {
						hid->endpoint_in_maxpacketsize = USBH_HID_BUFFER;
					}
				}
			}
		}
		break;

	case USB_DT_HID:
		{
			const struct hid_report_decriptor *desc = (const struct hid_report_decriptor *)descriptor;
			if (desc->header.bNumDescriptors > 0 && desc->report_descriptors_info[0].bDescriptorType == USB_DT_REPORT) {
				hid->report0_length = desc->report_descriptors_info[0].wDescriptorLength;
			}
		}
		break;

	default:
		break;
	}

	if (hid->endpoint_in_address && hid->report0_length) {
		hid->state_next = STATE_GET_REPORT_DESCRIPTOR_READ_SETUP;
		return true;
	}

	return false;
}

static void report_event(usbh_device_t *dev, usbh_packet_callback_data_t cb_data)
{
	(void)cb_data;// UNUSED

	hid_device_t *hid = (hid_device_t *)dev->drvdata;
	hid->report_state = REPORT_STATE_READY;
}

static void event(usbh_device_t *dev, usbh_packet_callback_data_t cb_data)
{
	hid_device_t *hid = (hid_device_t *)dev->drvdata;

	switch (hid->state_next) {
	case STATE_READING_COMPLETE_AND_CHECK_REPORT:
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
			case USBH_PACKET_CALLBACK_STATUS_ERRSIZ:
				if (hid_config.hid_in_message_handler) {
					hid_config.hid_in_message_handler(hid->device_id, hid->buffer, cb_data.transferred_length);
				}
				hid->state_next = STATE_READING_REQUEST;
				break;

			default:
				ERROR(cb_data.status);
				hid->state_next = STATE_INACTIVE;
				break;
			}
		}
		break;

	case STATE_GET_REPORT_DESCRIPTOR_READ_COMPLETE: // read complete, SET_IDLE to 0
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				LOG_PRINTF("READ REPORT COMPLETE \n");
				hid->state_next = STATE_READING_REQUEST;
				hid->endpoint_in_toggle = 0;

				parse_report_descriptor(hid, hid->buffer, cb_data.transferred_length);
				break;

			default:
				ERROR(cb_data.status);
				hid->state_next = STATE_INACTIVE;
				break;
			}
		}
		break;

	default:
		break;
	}
}


static void read_hid_in_endpoint(void *drvdata)
{
	hid_device_t *hid = (hid_device_t *)drvdata;
	usbh_packet_t packet;

	packet.address = hid->usbh_device->address;
	packet.data.in = &hid->buffer[0];
	packet.datalen = hid->endpoint_in_maxpacketsize;
	packet.endpoint_address = hid->endpoint_in_address;
	packet.endpoint_size_max = hid->endpoint_in_maxpacketsize;
	packet.endpoint_type = USBH_ENDPOINT_TYPE_INTERRUPT;
	packet.speed = hid->usbh_device->speed;
	packet.callback = event;
	packet.callback_arg = hid->usbh_device;
	packet.toggle = &hid->endpoint_in_toggle;

	hid->state_next = STATE_READING_COMPLETE_AND_CHECK_REPORT;
	usbh_read(hid->usbh_device, &packet);
}

/**
 * @param time_curr_us - monotically rising time
 *		unit is microseconds
 * @see usbh_poll()
 */
static void poll(void *drvdata, uint32_t time_curr_us)
{
	(void)time_curr_us;

	hid_device_t *hid = (hid_device_t *)drvdata;
	usbh_device_t *dev = hid->usbh_device;
	switch (hid->state_next) {
	case STATE_READING_REQUEST:
		{
			read_hid_in_endpoint(drvdata);
		}
		break;

	case STATE_GET_REPORT_DESCRIPTOR_READ_SETUP:
		{
			hid->endpoint_in_toggle = 0;
			// We support only the first report descriptor with index 0

			// limit the size of the report descriptor!
			if (hid->report0_length > USBH_HID_BUFFER) {
				hid->report0_length = USBH_HID_BUFFER;
			}

			struct usb_setup_data setup_data;

			setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_INTERFACE;
			setup_data.bRequest = USB_REQ_GET_DESCRIPTOR;
			setup_data.wValue = USB_DT_REPORT << 8;
			setup_data.wIndex = 0;
			setup_data.wLength = hid->report0_length;

			hid->state_next = STATE_GET_REPORT_DESCRIPTOR_READ_COMPLETE;
			device_control(dev, event, &setup_data, hid->buffer);
		}
		break;

	default:
		// do nothing - probably transfer is in progress
		break;
	}
}

static void remove(void *drvdata)
{
	hid_device_t *hid = (hid_device_t *)drvdata;
	hid->state_next = STATE_INACTIVE;
	hid->endpoint_in_address = 0;
}

bool hid_set_report(uint8_t device_id, uint8_t val)
{
	if (device_id >= USBH_HID_MAX_DEVICES) {
		LOG_PRINTF("invalid device id");
		return false;
	}

	hid_device_t *hid = &hid_device[device_id];
	if (hid->report_state != REPORT_STATE_READY) {
		LOG_PRINTF("reporting is not ready\n");
		// store and update afterwards
		return false;
	}

	if (hid->report_data_length == 0) {
		LOG_PRINTF("reporting is not available (report len=0)\n");
		return false;
	}

	struct usb_setup_data setup_data;
	setup_data.bmRequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;
	setup_data.bRequest = USB_HID_SET_REPORT;
	setup_data.wValue = 0x02 << 8;
	setup_data.wIndex = hid->interface_number;
	setup_data.wLength = hid->report_data_length;

	hid->report_data[0] = val;

	hid->report_state = REPORT_STATE_PENDING;
	device_control(hid->usbh_device, report_event, &setup_data, &hid->report_data);
	return true;
}

bool hid_is_connected(uint8_t device_id)
{
	if (device_id >= USBH_HID_MAX_DEVICES) {
		LOG_PRINTF("is connected: invalid device id");
		return false;
	}
	return hid_device[device_id].state_next == STATE_INACTIVE;
}


enum HID_TYPE hid_get_type(uint8_t device_id)
{
	if (hid_is_connected(device_id)) {
		return HID_TYPE_NONE;
	}
	return hid_device[device_id].hid_type;
}

static const usbh_dev_driver_info_t driver_info = {
	.deviceClass = -1,
	.deviceSubClass = -1,
	.deviceProtocol = -1,
	.idVendor = -1,
	.idProduct = -1,
	.ifaceClass = 0x03, // HID class
	.ifaceSubClass = -1,
	.ifaceProtocol = -1, // Do not care
};

const usbh_dev_driver_t usbh_hid_driver = {
	.init = init,
	.analyze_descriptor = analyze_descriptor,
	.poll = poll,
	.remove = remove,
	.info = &driver_info
};



