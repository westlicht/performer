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

#include "driver/usbh_device_driver.h"
#include "usbh_driver_ac_midi_private.h"
#include "usart_helpers.h"

#include <stddef.h>

#include <libopencm3/usb/midi.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/usbstd.h>

static midi_device_t midi_device[USBH_AC_MIDI_MAX_DEVICES];
static const midi_config_t *midi_config = NULL;
static bool initialized = false;

void midi_driver_init(const midi_config_t *config)
{
	uint32_t i;
	midi_config = config;
	for (i = 0; i < USBH_AC_MIDI_MAX_DEVICES; i++) {
		midi_device[i].state = 0;
	}
	initialized = true;
}
/**
 *
 *
 */
static void *init(usbh_device_t *usbh_dev, const usbh_dev_driver_info_t * device_info)
{
	if (!midi_config || !initialized) {
		LOG_PRINTF("\n%s/%d : driver not initialized\n", __FILE__, __LINE__);
		return 0;
	}
	uint32_t i;
	midi_device_t *drvdata = NULL;

	// find free data space for midi device
	for (i = 0; i < USBH_AC_MIDI_MAX_DEVICES; i++) {
		if (midi_device[i].state == 0) {
			drvdata = &midi_device[i];
			drvdata->vendor_id = device_info->idVendor;
			drvdata->product_id = device_info->idProduct;
			drvdata->device_id = i;
			drvdata->endpoint_in_address = 0;
			drvdata->endpoint_out_address = 0;
			drvdata->endpoint_in_toggle = 0;
			drvdata->endpoint_out_toggle = 0;
			drvdata->usbh_device = usbh_dev;
			drvdata->write_callback_user = NULL;
			drvdata->sending = false;
			break;
		}
	}

	return drvdata;
}

/**
 * Returns true if all needed data are parsed
 */
static bool analyze_descriptor(void *drvdata, void *descriptor)
{
	midi_device_t *midi = drvdata;
	uint8_t desc_type = ((uint8_t *)descriptor)[1];
	switch (desc_type) {
	case USB_DT_CONFIGURATION:
		{
			struct usb_config_descriptor *cfg =
				(struct usb_config_descriptor*)descriptor;
			midi->buffer[0] = cfg->bConfigurationValue;
		}
		break;
	case USB_DT_DEVICE:
		break;
	case USB_DT_INTERFACE:
		break;
	case USB_DT_ENDPOINT:
		{
			struct usb_endpoint_descriptor *ep =
				(struct usb_endpoint_descriptor*)descriptor;
            // allow for both bulk and interrupt endpoints
			if (
                (ep->bmAttributes&0x03) == USB_ENDPOINT_ATTR_BULK ||
			    (ep->bmAttributes&0x03) == USB_ENDPOINT_ATTR_INTERRUPT
            ) {
				uint8_t epaddr = ep->bEndpointAddress;
				if (epaddr & (1<<7)) {
					midi->endpoint_in_address = epaddr&0x7f;
					if (ep->wMaxPacketSize < USBH_AC_MIDI_BUFFER) {
						midi->endpoint_in_maxpacketsize = ep->wMaxPacketSize;
					} else {
						midi->endpoint_in_maxpacketsize = USBH_AC_MIDI_BUFFER;
					}
				} else {
					midi->endpoint_out_address = epaddr;
					midi->endpoint_out_maxpacketsize = ep->wMaxPacketSize;
				}

				if (midi->endpoint_in_address && midi->endpoint_out_address) {
					midi->state = 1;
					return true;
				}
			}
		}
		break;

	case USB_AUDIO_DT_CS_ENDPOINT:
	{
		struct usb_midi_in_jack_descriptor *midi_in_jack_desc =
			(struct usb_midi_in_jack_descriptor *) descriptor;
		(void)midi_in_jack_desc;
	}
		break;
	// TODO Class Specific descriptors
	default:
		break;
	}
	return false;
}

static void midi_in_message(midi_device_t *midi, const uint8_t datalen)
{
	uint8_t i = 0;
	if (midi_config->read_callback) {
		for (i = 0; i < datalen; i += 4) {

//			uint8_t cable_number = (midi->buffer[i] & 0xf0) >> 4;
			uint8_t code_id = midi->buffer[i]&0xf;

			uint8_t *ptrdata = &midi->buffer[i];
			if (code_id < 2) {
				continue;
			}
			midi_config->read_callback(midi->device_id, ptrdata);
		}
	}
}

static void event(usbh_device_t *dev, usbh_packet_callback_data_t status)
{
	midi_device_t *midi = (midi_device_t *)dev->drvdata;
	switch (midi->state) {
	case 26:
		{
			switch (status.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				midi_in_message(midi, midi->endpoint_in_maxpacketsize);
				midi->state = 25;
				break;

			case USBH_PACKET_CALLBACK_STATUS_ERRSIZ:
				midi_in_message(midi, status.transferred_length);
				midi->state = 25;
				break;

			default:
				LOG_PRINTF("FATAL ERROR, MIDI DRIVER DEAD \n");
				//~ dev->drv->remove();
				midi->state = 0;
				break;
			}
		}
		break;

	case 102:
		{
			midi->state = 101;
			LOG_PRINTF("\n CAN'T TOUCH THIS... ignoring data\n");
		}
		break;

	default:
		break;
	}
}


static void read_midi_in(void *drvdata, const uint8_t nextstate)
{
	midi_device_t *midi = drvdata;
	usbh_packet_t packet;

	packet.address = midi->usbh_device->address;
	packet.data.in = &midi->buffer[0];
	packet.datalen = midi->endpoint_in_maxpacketsize;
	packet.endpoint_address = midi->endpoint_in_address;
	packet.endpoint_size_max = midi->endpoint_in_maxpacketsize;
	packet.endpoint_type = USBH_ENDPOINT_TYPE_BULK;
	packet.speed = midi->usbh_device->speed;
	packet.callback = event;
	packet.callback_arg = midi->usbh_device;
	packet.toggle = &midi->endpoint_in_toggle;

	midi->state = nextstate;
	usbh_read(midi->usbh_device,&packet);
}

/**
 *
 *  @param t_us global time us
 */
static void poll(void *drvdata, uint32_t t_us)
{
	(void)drvdata;

	midi_device_t *midi = drvdata;
	switch (midi->state) {

	/// Upon configuration, some controllers send additional error data
	/// case 100, 101, 102 cares for ignoring those data
	case 100:
		{
			midi->time_us_config = t_us;
			midi->state = 101;
		}
		break;

	case 101:
		{
			read_midi_in(drvdata, 102);
		}
		break;

	case 102:
		{
			// if elapsed MIDI initial delay microseconds
			if (t_us - midi->time_us_config > MIDI_INITIAL_DELAY) {
				midi->state = 26;
			}
		}
		break;

	case 25:
		{
			read_midi_in(drvdata, 26);
		}
		break;

	case 1:
		{
			midi->state = 100;

			midi->endpoint_in_toggle = 0;
			LOG_PRINTF("\nMIDI CONFIGURED\n");

			// Notify user
			if (midi_config->notify_connected) {
				midi_config->notify_connected(midi->device_id, midi->vendor_id, midi->product_id, midi->endpoint_out_maxpacketsize);
			}
		}
		break;
	}
}

// don't call directly
static void write_callback(usbh_device_t *dev, usbh_packet_callback_data_t status)
{
	(void)status;
	midi_device_t *midi = (midi_device_t *)dev->drvdata;

	if (midi->sending) {
		midi->sending = false;
		const midi_write_callback_t callback = midi->write_callback_user;
		if (!callback) {
			return;
		}

		if (status.status & USBH_PACKET_CALLBACK_STATUS_OK) {
			callback(midi->write_packet.datalen);
		} else {
			if (status.status & USBH_PACKET_CALLBACK_STATUS_ERRSIZ) {
				const uint32_t length = status.transferred_length;
				callback(length);
			} else {
				callback(0);
			}
		}
	}
}

void usbh_midi_write(uint8_t device_id, const void *data, uint32_t length, midi_write_callback_t callback)
{
	// bad device_id handling
	if (device_id >= USBH_AC_MIDI_MAX_DEVICES) {
		return;
	}

	midi_device_t *midi = &midi_device[device_id];

	// device with provided device_id is not alive
	if (midi->state == 0) {
		return;
	}

	usbh_device_t *dev = midi->usbh_device;
	if (midi->endpoint_out_address == 0) {
		return;
	}

	midi->sending = true;
	midi->write_callback_user = callback;

	midi->write_packet.data.out = data;
	midi->write_packet.datalen = length;
	midi->write_packet.address = dev->address;
	midi->write_packet.endpoint_address = midi->endpoint_out_address;
	midi->write_packet.endpoint_size_max = midi->endpoint_out_maxpacketsize;
	midi->write_packet.endpoint_type = USBH_ENDPOINT_TYPE_BULK;
	midi->write_packet.speed = dev->speed;
	midi->write_packet.callback = write_callback;
	midi->write_packet.callback_arg = midi->usbh_device;
	midi->write_packet.toggle = &midi->endpoint_out_toggle;


	usbh_write(dev, &midi->write_packet);
}

static void remove(void *drvdata)
{
	midi_device_t *midi = drvdata;

	if (midi_config->notify_disconnected) {
		midi_config->notify_disconnected(midi->device_id);
	}

	midi->state = 0;
	midi->endpoint_in_address = 0;
	midi->endpoint_out_address = 0;
}

static const usbh_dev_driver_info_t usbh_midi_driver_info = {
	.deviceClass = -1,
	.deviceSubClass = -1,
	.deviceProtocol = -1,
	.idVendor = -1,
	.idProduct = -1,
	.ifaceClass = 0x01,
	.ifaceSubClass = 0x03,
	.ifaceProtocol = -1,
};

// [1][2][4][36][4][36][36][36][5][37][5][37]

const usbh_dev_driver_t usbh_midi_driver = {
	.init = init,
	.analyze_descriptor = analyze_descriptor,
	.poll = poll,
	.remove = remove,
	.info = &usbh_midi_driver_info
};
