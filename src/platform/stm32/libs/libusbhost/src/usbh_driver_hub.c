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

#include "usbh_driver_hub_private.h"
#include "driver/usbh_device_driver.h"
#include "usart_helpers.h"
#include "usbh_config.h"

#include <stddef.h>
#include <stdint.h>

static hub_device_t hub_device[USBH_MAX_HUBS];

static bool initialized = false;

void hub_driver_init(void)
{
	uint32_t i;

	initialized = true;

	for (i = 0; i < USBH_MAX_HUBS; i++) {
		hub_device[i].device[0] = 0;
		hub_device[i].ports_num = 0;
		hub_device[i].current_port = CURRENT_PORT_NONE;
	}
}

static void *init(usbh_device_t *usbh_dev, const usbh_dev_driver_info_t * device_info)
{
	if (!initialized) {
		LOG_PRINTF("\n%s/%d : driver not initialized\n", __FILE__, __LINE__);
		return 0;
	}

	uint32_t i;
	hub_device_t *drvdata = NULL;
	// find free data space for hub device
	for (i = 0; i < USBH_MAX_HUBS; i++) {
		if (hub_device[i].device[0] == 0) {
			break;
		}
	}
	LOG_PRINTF("{%d}",i);
	if (i == USBH_MAX_HUBS) {
		LOG_PRINTF("Unable to initialize hub driver");
		return 0;
	}

	drvdata = &hub_device[i];
	drvdata->state = EVENT_STATE_NONE;
	drvdata->ports_num = 0;
	drvdata->device[0] = usbh_dev;
	drvdata->busy = 0;
	drvdata->endpoint_in_address = 0;
	drvdata->endpoint_in_maxpacketsize = 0;

	return drvdata;
}

/**
 * @returns true if all needed data are parsed
 */
static bool analyze_descriptor(void *drvdata, void *descriptor)
{
	hub_device_t *hub = (hub_device_t *)drvdata;
	uint8_t desc_type = ((uint8_t *)descriptor)[1];
	switch (desc_type) {
	case USB_DT_ENDPOINT:
		{
			struct usb_endpoint_descriptor *ep = (struct usb_endpoint_descriptor *)descriptor;
			if ((ep->bmAttributes&0x03) == USB_ENDPOINT_ATTR_INTERRUPT) {
				uint8_t epaddr = ep->bEndpointAddress;
				if (epaddr & (1<<7)) {
					hub->endpoint_in_address = epaddr&0x7f;
					hub->endpoint_in_maxpacketsize = ep->wMaxPacketSize;
				}
			}
			LOG_PRINTF("ENDPOINT DESCRIPTOR FOUND\n");
		}
		break;

	case USB_DT_HUB:
		{
			struct usb_hub_descriptor *desc = (struct usb_hub_descriptor *)descriptor;
			if ( desc->head.bNbrPorts <= USBH_HUB_MAX_DEVICES) {
				hub->ports_num = desc->head.bNbrPorts;
			} else {
				LOG_PRINTF("INCREASE NUMBER OF ENABLED PORTS\n");
				hub->ports_num = USBH_HUB_MAX_DEVICES;
			}
			LOG_PRINTF("HUB DESCRIPTOR FOUND \n");
		}
		break;

	default:
		LOG_PRINTF("TYPE: %02X \n",desc_type);
		break;
	}

	if (hub->endpoint_in_address) {
		hub->state = EVENT_STATE_INITIAL;
		LOG_PRINTF("end enum");
		return true;
	}
	return false;
}

// Enumerate
static void event(usbh_device_t *dev, usbh_packet_callback_data_t cb_data)
{
	hub_device_t *hub = (hub_device_t *)dev->drvdata;

	LOG_PRINTF("\nHUB->STATE = %d\n", hub->state);
	switch (hub->state) {
	case EVENT_STATE_POLL:
		switch (cb_data.status) {
		case USBH_PACKET_CALLBACK_STATUS_OK:
			{
				uint8_t i;
				uint8_t *buf = hub->buffer;
				uint32_t psc = 0;	// Limit: up to 4 bytes...
				for (i = 0; i < cb_data.transferred_length; i++) {
					psc += buf[i] << (i*8);
				}
				int8_t port = 0;

				LOG_PRINTF("psc:%d\n",psc);
				// Driver error... port not found
				if (!psc) {
					// Continue reading status change endpoint
					hub->state = EVENT_STATE_POLL_REQ;
					break;
				}

				for (i = 0; i <= hub->ports_num; i++) {
					if (psc & (1<<i)) {
						port = i;
						psc = 0;
						break;
					}
				}

				if (hub->current_port >= 1) {
					if (hub->current_port != port) {
						LOG_PRINTF("X");
						hub->state = EVENT_STATE_POLL_REQ;
						break;
					}
				}
				struct usb_setup_data setup_data;
				// If regular port event, else hub event
				if (port) {
					setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
				} else {
					setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE;
				}

				setup_data.bRequest = USB_REQ_GET_STATUS;
				setup_data.wValue = 0;
				setup_data.wIndex = port;
				setup_data.wLength = 4;
				hub->state = EVENT_STATE_GET_STATUS_COMPLETE;

				hub->current_port = port;
				LOG_PRINTF("\n\nPORT FOUND: %d\n", port);
				device_control(dev, event, &setup_data, &hub->hub_and_port_status[port]);
			}
			break;

		default:
			ERROR(cb_data.status);
			hub->state = EVENT_STATE_NONE;
			break;

		case USBH_PACKET_CALLBACK_STATUS_EAGAIN:

			// In case of EAGAIN error, retry read on status endpoint
			hub->state = EVENT_STATE_POLL_REQ;
			LOG_PRINTF("HUB: Retrying...\n");
			break;
		}
		break;

	case EVENT_STATE_READ_HUB_DESCRIPTOR_COMPLETE:// Hub descriptor found
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				{
					struct usb_hub_descriptor *hub_descriptor =
						(struct usb_hub_descriptor *)hub->buffer;

					// Check size
					if (hub_descriptor->head.bDescLength > hub->desc_len) {
						struct usb_setup_data setup_data;
						hub->desc_len = hub_descriptor->head.bDescLength;

						setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE;
						setup_data.bRequest = USB_REQ_GET_DESCRIPTOR;
						setup_data.wValue = 0x29<<8;
						setup_data.wIndex = 0;
						setup_data.wLength = hub->desc_len;

						hub->state = EVENT_STATE_READ_HUB_DESCRIPTOR_COMPLETE;
						device_control(dev, event, &setup_data, hub->buffer);
						break;
					} else if (hub_descriptor->head.bDescLength == hub->desc_len) {
						hub->ports_num = hub_descriptor->head.bNbrPorts;

						hub->state = EVENT_STATE_ENABLE_PORTS;
						hub->index = 0;
						cb_data.status = USBH_PACKET_CALLBACK_STATUS_OK;
						event(dev, cb_data);
					} else {
						//try again
					}
				}
				break;

			case USBH_PACKET_CALLBACK_STATUS_ERRSIZ:
				{
					LOG_PRINTF("->\t\t\t\t\t ERRSIZ: deschub\n");
					struct usb_hub_descriptor*hub_descriptor =
						(struct usb_hub_descriptor *)hub->buffer;

					if (cb_data.transferred_length >= sizeof(struct usb_hub_descriptor_head)) {
						if (cb_data.transferred_length == hub_descriptor->head.bDescLength) {
							// Process HUB descriptor
							if ( hub_descriptor->head.bNbrPorts <= USBH_HUB_MAX_DEVICES) {
								hub->ports_num = hub_descriptor->head.bNbrPorts;
							} else {
								LOG_PRINTF("INCREASE NUMBER OF ENABLED PORTS\n");
								hub->ports_num = USBH_HUB_MAX_DEVICES;
							}
							hub->state = EVENT_STATE_ENABLE_PORTS;
							hub->index = 0;

							cb_data.status = USBH_PACKET_CALLBACK_STATUS_OK;
							event(dev, cb_data);
						}
					}
				}
				break;

			default:
				ERROR(cb_data.status);
				break;
			}
		}
		break;

	case EVENT_STATE_ENABLE_PORTS:// enable ports
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				if (hub->index < hub->ports_num) {
					hub->index++;
					struct usb_setup_data setup_data;

					LOG_PRINTF("[!%d!]",hub->index);
					setup_data.bmRequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
					setup_data.bRequest = HUB_REQ_SET_FEATURE;
					setup_data.wValue = HUB_FEATURE_PORT_POWER;
					setup_data.wIndex = hub->index;
					setup_data.wLength = 0;

					device_control(dev, event, &setup_data, 0);
				} else {
					// TODO:
					// Delay Based on hub descriptor field bPwr2PwrGood
					// delay_ms_busy_loop(200);

					LOG_PRINTF("\nHUB CONFIGURED & PORTS POWERED\n");

					// get device status
					struct usb_setup_data setup_data;

					setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE;
					setup_data.bRequest = USB_REQ_GET_STATUS;
					setup_data.wValue = 0;
					setup_data.wIndex = 0;
					setup_data.wLength = 4;

					hub->state = EVENT_STATE_GET_PORT_STATUS;
					hub->index = 0;
					device_control(dev, event, &setup_data, hub->buffer);
				}
				break;

			default:
				ERROR(cb_data.status);
				break;
			}
		}
		break;

	case EVENT_STATE_GET_PORT_STATUS:
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				{
					if (hub->index < hub->ports_num) {
						//TODO: process data contained in hub->buffer

						struct usb_setup_data setup_data;

						setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
						setup_data.bRequest = USB_REQ_GET_STATUS;
						setup_data.wValue = 0;
						setup_data.wIndex = ++hub->index;
						setup_data.wLength = 4;

						hub->state = EVENT_STATE_GET_PORT_STATUS;
						device_control(dev, event, &setup_data, hub->buffer);
					} else {
						hub->busy = 0;
						hub->state = EVENT_STATE_POLL_REQ;
					}

				}
				break;

			default:
				ERROR(cb_data.status);
				break;
			}
		}
		break;

	case EVENT_STATE_GET_STATUS_COMPLETE:
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				{
					int8_t port = hub->current_port;
					LOG_PRINTF("|%d",port);


					// Get Port status, else Get Hub status
					if (port) {
						uint16_t stc = hub->hub_and_port_status[port].stc;

						// Connection status changed
						if (stc & (1<<HUB_FEATURE_PORT_CONNECTION)) {

							// Check, whether device is in connected state
							if (!hub->device[port]) {
								if (!usbh_enum_available() || hub->busy) {
									LOG_PRINTF("\n\t\t\tCannot enumerate %d %d\n", !usbh_enum_available(), hub->busy);
									hub->state = EVENT_STATE_POLL_REQ;
									break;
								}
							}

							// clear feature C_PORT_CONNECTION
							struct usb_setup_data setup_data;

							setup_data.bmRequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
							setup_data.bRequest = HUB_REQ_CLEAR_FEATURE;
							setup_data.wValue = HUB_FEATURE_C_PORT_CONNECTION;
							setup_data.wIndex = port;
							setup_data.wLength = 0;

							hub->state = EVENT_STATE_PORT_RESET_REQ;
							device_control(dev, event, &setup_data, 0);

						} else if(stc & (1<<HUB_FEATURE_PORT_RESET)) {
							// clear feature C_PORT_RESET
							// Reset processing is complete, enumerate device
							struct usb_setup_data setup_data;

							setup_data.bmRequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
							setup_data.bRequest = HUB_REQ_CLEAR_FEATURE;
							setup_data.wValue = HUB_FEATURE_C_PORT_RESET;
							setup_data.wIndex = port;
							setup_data.wLength = 0;

							hub->state = EVENT_STATE_PORT_RESET_COMPLETE;

							LOG_PRINTF("RESET");
							device_control(dev, event, &setup_data, 0);
						} else {
							LOG_PRINTF("another STC %d\n", stc);
						}
					} else {
						hub->state = EVENT_STATE_POLL_REQ;
						LOG_PRINTF("HUB status change\n");
					}
				}
				break;

			default:
				ERROR(cb_data.status);
				// continue
				hub->state = EVENT_STATE_POLL_REQ;
				break;
			}
		}
		break;
	case EVENT_STATE_PORT_RESET_REQ:
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				{
					int8_t port = hub->current_port;
					uint16_t stc = hub->hub_and_port_status[port].stc;
					if (!hub->device[port]) {
						if ((stc) & (1<<HUB_FEATURE_PORT_CONNECTION)) {
							struct usb_setup_data setup_data;

							setup_data.bmRequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
							setup_data.bRequest = HUB_REQ_SET_FEATURE;
							setup_data.wValue = HUB_FEATURE_PORT_RESET;
							setup_data.wIndex = port;
							setup_data.wLength = 0;

							hub->state = EVENT_STATE_GET_PORT_STATUS;

							LOG_PRINTF("CONN");

							hub->busy = 1;
							device_control(dev, event, &setup_data, 0);
						}
					} else {
						LOG_PRINTF("\t\t\t\tDISCONNECT EVENT\n");
						device_remove(hub->device[port]);

						hub->device[port] = 0;
						hub->current_port = CURRENT_PORT_NONE;
						hub->state = EVENT_STATE_POLL_REQ;
						hub->busy = 0;
					}
				}
				break;

			default:
				ERROR(cb_data.status);
				// continue
				hub->state = EVENT_STATE_POLL_REQ;
				break;
			}
		}
		break;
	case EVENT_STATE_PORT_RESET_COMPLETE:	// RESET COMPLETE, start enumeration
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				{
					LOG_PRINTF("\nPOLL\n");
					int8_t port = hub->current_port;
					uint16_t sts = hub->hub_and_port_status[port].sts;


					if (sts & (1<<HUB_FEATURE_PORT_ENABLE)) {
						hub->device[port] = usbh_get_free_device(dev);

						if (!hub->device[port]) {
							LOG_PRINTF("\nFATAL ERROR\n");
							return;// DEAD END
						}
						if ((sts & (1<<(HUB_FEATURE_PORT_LOWSPEED))) &&
							!(sts & (1<<(HUB_FEATURE_PORT_HIGHSPEED)))) {
#define DISABLE_LOW_SPEED
#ifdef DISABLE_LOW_SPEED
							LOG_PRINTF("Low speed device");

							// Disable Low speed device immediately
							struct usb_setup_data setup_data;

							setup_data.bmRequestType = USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE | USB_REQ_TYPE_ENDPOINT;
							setup_data.bRequest = HUB_REQ_CLEAR_FEATURE;
							setup_data.wValue = HUB_FEATURE_PORT_ENABLE;
							setup_data.wIndex = port;
							setup_data.wLength = 0;

							// After write process another devices, poll for events
							//Expecting all ports are powered (constant/non-changeable after init)
							hub->state = EVENT_STATE_GET_PORT_STATUS;

							hub->current_port = CURRENT_PORT_NONE;
							device_control(dev, event, &setup_data, 0);
#else
							hub->device[port]->speed = USBH_SPEED_LOW;
							LOG_PRINTF("Low speed device");
							hub->timestamp_us = hub->time_curr_us;
							hub->state = EVENT_STATE_SLEEP_500_MS; // schedule wait for 500ms
#endif
						} else if (!(sts & (1<<(HUB_FEATURE_PORT_LOWSPEED))) &&
							!(sts & (1<<(HUB_FEATURE_PORT_HIGHSPEED)))) {
							hub->device[port]->speed = USBH_SPEED_FULL;
							LOG_PRINTF("Full speed device");
							hub->timestamp_us = hub->time_curr_us;
							hub->state = EVENT_STATE_SLEEP_500_MS; // schedule wait for 500ms
						}


					} else {
						LOG_PRINTF("%s:%d Do not know what to do, when device is disabled after reset\n", __FILE__, __LINE__);
						hub->state = EVENT_STATE_POLL_REQ;
						return;
					}
				}
				break;

			default:
				ERROR(cb_data.status);
				// continue
				hub->state = EVENT_STATE_POLL_REQ;
				break;
			}
		}
		break;
	default:
		LOG_PRINTF("UNHANDLED EVENT %d\n",hub->state);
		break;
	}
}

static void read_ep1(void *drvdata)
{
	hub_device_t *hub = (hub_device_t *)drvdata;
	usbh_packet_t packet;

	packet.address = hub->device[0]->address;
	packet.data.in = hub->buffer;
	packet.datalen = hub->endpoint_in_maxpacketsize;
	packet.endpoint_address = hub->endpoint_in_address;
	packet.endpoint_size_max = hub->endpoint_in_maxpacketsize;
	packet.endpoint_type = USBH_ENDPOINT_TYPE_INTERRUPT;
	packet.speed = hub->device[0]->speed;
	packet.callback = event;
	packet.callback_arg = hub->device[0];
	packet.toggle = &hub->endpoint_in_toggle;

	hub->state = EVENT_STATE_POLL;
	usbh_read(hub->device[0], &packet);
	LOG_PRINTF("@hub %d/EP1 |  \n", hub->device[0]->address);

}

/**
 * @param time_curr_us - monotically rising time
 *		unit is microseconds
 * @see usbh_poll()
 */
static void poll(void *drvdata, uint32_t time_curr_us)
{
	hub_device_t *hub = (hub_device_t *)drvdata;
	usbh_device_t *dev = hub->device[0];

	hub->time_curr_us = time_curr_us;

	switch (hub->state) {
	case EVENT_STATE_POLL_REQ:
		{
			if (usbh_enum_available()) {
				read_ep1(hub);
			} else {
				LOG_PRINTF("enum not available\n");
			}
		}
		break;

	case EVENT_STATE_INITIAL:
		{
			if (hub->ports_num) {
				hub->index = 0;
				hub->state = EVENT_STATE_ENABLE_PORTS;
				LOG_PRINTF("No need to get HUB DESC\n");
				event(dev, (usbh_packet_callback_data_t){0, 0});
			} else {
				hub->endpoint_in_toggle = 0;

				struct usb_setup_data setup_data;
				hub->desc_len = hub->device[0]->packet_size_max0;

				setup_data.bmRequestType = USB_REQ_TYPE_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE;
				setup_data.bRequest = USB_REQ_GET_DESCRIPTOR;
				setup_data.wValue = 0x29 << 8;
				setup_data.wIndex = 0;
				setup_data.wLength = hub->desc_len;

				hub->state = EVENT_STATE_READ_HUB_DESCRIPTOR_COMPLETE;
				device_control(dev, event, &setup_data, hub->buffer);
				LOG_PRINTF("DO Need to get HUB DESC\n");
			}
		}
		break;
	case EVENT_STATE_SLEEP_500_MS:
		if (hub->time_curr_us - hub->timestamp_us > 500000) {
			int8_t port = hub->current_port;
			LOG_PRINTF("PORT: %d\n", port);
			LOG_PRINTF("NEW device at address: %d\n", hub->device[port]->address);
			hub->device[port]->lld = hub->device[0]->lld;

			device_enumeration_start(hub->device[port]);
			hub->current_port = CURRENT_PORT_NONE;

			// Maybe error, when assigning address is taking too long
			//
			// Detail:
			// USB hub cannot enable another port while the device
			// the current one is also in address state (has address==0)
			// Only one device on bus can have address==0
			hub->busy = 0;

			hub->state = EVENT_STATE_POLL_REQ;
		}
		break;
	default:
		break;
	}

	if (usbh_enum_available()) {
		uint32_t i;
		for (i = 1; i < USBH_HUB_MAX_DEVICES + 1; i++) {
			if (hub->device[i]) {
				if (hub->device[i]->drv && hub->device[i]->drvdata) {
					hub->device[i]->drv->poll(hub->device[i]->drvdata, time_curr_us);
				}
			}
		}
	}
}
static void remove(void *drvdata)
{
	hub_device_t *hub = (hub_device_t *)drvdata;
	uint8_t i;

	hub->state = EVENT_STATE_NONE;
	hub->endpoint_in_address = 0;
	hub->busy = 0;
	for (i = 0; i < USBH_HUB_MAX_DEVICES + 1; i++) {
		hub->device[i] = 0;

	}
}

static const usbh_dev_driver_info_t driver_info = {
	.deviceClass = 0x09,
	.deviceSubClass = -1,
	.deviceProtocol = -1,
	.idVendor = -1,
	.idProduct = -1,
	.ifaceClass = 0x09,
	.ifaceSubClass = -1,
	.ifaceProtocol = -1
};

const usbh_dev_driver_t usbh_hub_driver = {
	.init = init,
	.analyze_descriptor = analyze_descriptor,
	.poll = poll,
	.remove = remove,
	.info = &driver_info
};
