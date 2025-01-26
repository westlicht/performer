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

#include "driver/usbh_device_driver.h"
#include "usbh_lld_stm32f4.h"
#include "usart_helpers.h"

#include <string.h>
#include <stdint.h>
#include <libopencm3/usb/dwc/otg_hs.h>
#include <libopencm3/usb/dwc/otg_fs.h>



/* Receive FIFO size in 32-bit words. */
#define RX_FIFO_SIZE    (64)
/* Transmit NON-periodic FIFO size in 32-bit words. */
#define TX_NP_FIFO_SIZE (64)
/* Transmit periodic FIFO size in 32-bit words. */
#define TX_P_FIFO_SIZE  (64)

enum CHANNEL_STATE {
	CHANNEL_STATE_FREE = 0,
	CHANNEL_STATE_WORK = 1
};

struct _channel {
	enum CHANNEL_STATE state;
	usbh_packet_t packet;
	uint32_t data_index; //used in receive function
};
typedef struct _channel channel_t;

enum DEVICE_STATE {
	DEVICE_STATE_INIT = 0,
	DEVICE_STATE_RUN = 1,
	DEVICE_STATE_RESET = 2
};

enum DEVICE_POLL_STATE {
	DEVICE_POLL_STATE_DISCONN = 0,
	DEVICE_POLL_STATE_DEVCONN = 1,
	DEVICE_POLL_STATE_DEVRST = 2,
	DEVICE_POLL_STATE_RUN = 3
};

struct _usbh_lld_stm32f4_driver_data {
	usbh_generic_data_t generic;
	const uint32_t base;
	channel_t *channels;
	const uint8_t num_channels;

	uint32_t poll_sequence;
	enum DEVICE_POLL_STATE dpstate;
	enum DEVICE_STATE state;
	uint32_t state_prev;//for reset only
	uint32_t time_curr_us;
	uint32_t timestamp_us;
};
typedef struct _usbh_lld_stm32f4_driver_data usbh_lld_stm32f4_driver_data_t;



/*
 * Define correct REBASE. If only one driver is enabled use directly OTG base
 *
 */
#if 	defined(USE_STM32F4_USBH_DRIVER_FS) || \
		defined(USE_STM32F4_USBH_DRIVER_HS)

#if 	defined(USE_STM32F4_USBH_DRIVER_FS) && \
		defined(USE_STM32F4_USBH_DRIVER_HS)
#define REBASE(reg)				MMIO32(dev->base + reg)
#define REBASE_CH(reg, x)	MMIO32(dev->base + reg(x))
#elif defined(USE_STM32F4_USBH_DRIVER_FS)
#define REBASE(reg)				MMIO32(USB_OTG_FS_BASE + reg)
#define REBASE_CH(reg, x)	MMIO32(USB_OTG_FS_BASE + reg(x))
#elif defined(USE_STM32F4_USBH_DRIVER_HS)
#define REBASE(reg)				MMIO32(USB_OTG_HS_BASE + reg)
#define REBASE_CH(reg, x)	MMIO32(USB_OTG_HS_BASE + reg(x))
#endif

static int8_t get_free_channel(void *drvdata);
static void channels_init(void *drvdata);
static void rxflvl_handle(void *drvdata);
static void free_channel(void *drvdata, uint8_t channel);





static inline void reset_start(usbh_lld_stm32f4_driver_data_t *dev)
{

	// apply reset condition on port
	REBASE(OTG_HPRT) |= OTG_HPRT_PRST;

	// push current state to stack
	dev->state_prev = dev->state;

	// move to new state
	dev->state = DEVICE_STATE_RESET;

	// schedule disable reset condition after ~10ms
	dev->timestamp_us = dev->time_curr_us;
}

/**
 * Should be nonblocking
 *
 */
static void init(void *drvdata)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	dev->state = DEVICE_STATE_INIT;
	dev->poll_sequence = 0;
	dev->timestamp_us = dev->time_curr_us;

	//Disable interrupts first
	REBASE(OTG_GAHBCFG) &= ~OTG_GAHBCFG_GINT;

	// Select full speed phy
	REBASE(OTG_GUSBCFG) |= OTG_GUSBCFG_PHYSEL;
}

static uint32_t usbh_to_stm32_endpoint_type(enum USBH_ENDPOINT_TYPE usbh_eptyp)
{
	switch (usbh_eptyp) {
	case USBH_ENDPOINT_TYPE_CONTROL: return OTG_HCCHAR_EPTYP_CONTROL;
	case USBH_ENDPOINT_TYPE_BULK: return OTG_HCCHAR_EPTYP_BULK;

	// Use bulk transfer also for interrupt, since no difference is on protocol layer
	// Except different behaviour of the core
	case USBH_ENDPOINT_TYPE_INTERRUPT: return OTG_HCCHAR_EPTYP_BULK;
	case USBH_ENDPOINT_TYPE_ISOCHRONOUS: return OTG_HCCHAR_EPTYP_ISOCHRONOUS;
	default:
		LOG_PRINTF("\n\n\n\nWRONG EP TYPE\n\n\n\n\n");
		return OTG_HCCHAR_EPTYP_CONTROL;
	}
}

static void stm32f4_usbh_port_channel_setup(
	void *drvdata, uint32_t channel, uint32_t epdir)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	channel_t *channels = dev->channels;
	uint32_t max_packet_size = channels[channel].packet.endpoint_size_max;
	uint32_t address = channels[channel].packet.address;
	uint32_t epnum = channels[channel].packet.endpoint_address;
	uint32_t eptyp = usbh_to_stm32_endpoint_type(channels[channel].packet.endpoint_type);

	uint32_t speed = 0;
	if (channels[channel].packet.speed == USBH_SPEED_LOW) {
		speed = OTG_HCCHAR_LSDEV;
	}

	REBASE_CH(OTG_HCCHAR, channel) = OTG_HCCHAR_CHENA |
				(OTG_HCCHAR_DAD_MASK & (address << 22)) |
				OTG_HCCHAR_MCNT_1 |
				(OTG_HCCHAR_EPTYP_MASK & (eptyp)) |
				(speed) |
				(OTG_HCCHAR_EPDIR_MASK & epdir) |
				(OTG_HCCHAR_EPNUM_MASK & (epnum << 11)) |
				(OTG_HCCHAR_MPSIZ_MASK & max_packet_size);

}


/**
 * TODO: Check for maximum datalength
 */
static void read(void *drvdata, usbh_packet_t *packet)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	channel_t *channels = dev->channels;

	int8_t channel = get_free_channel(dev);
	if (channel == -1) {
		// BIG PROBLEM
		LOG_PRINTF("FATAL ERROR IN, NO CHANNEL LEFT \n");
		usbh_packet_callback_data_t cb_data;
		cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
		cb_data.transferred_length = 0;
		packet->callback(packet->callback_arg, cb_data);
		return;
	}

	channels[channel].data_index = 0;
	channels[channel].packet = *packet;

	uint32_t dpid;
	if (packet->toggle[0]) {
		dpid = OTG_HCTSIZ_DPID_DATA1;
	} else {
		dpid = OTG_HCTSIZ_DPID_DATA0;
	}

	uint32_t num_packets;
	if (packet->datalen) {
		num_packets = ((packet->datalen - 1) / packet->endpoint_size_max) + 1;
	} else {
		num_packets = 0;
	}

	REBASE_CH(OTG_HCTSIZ, channel) = dpid | (num_packets << 19) | packet->datalen;

	stm32f4_usbh_port_channel_setup(dev, channel, OTG_HCCHAR_EPDIR_IN);
}

/**
 *
 * 	Bug: datalen > max_packet_size ...
 */
static void write(void *drvdata, const usbh_packet_t *packet)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	channel_t *channels = dev->channels;

	int8_t channel = get_free_channel(dev);

	if (channel == -1) {
		// BIG PROBLEM
		LOG_PRINTF("FATAL ERROR OUT, NO CHANNEL LEFT \n");
		usbh_packet_callback_data_t cb_data;
		cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
		cb_data.transferred_length = 0;
		packet->callback(packet->callback_arg, cb_data);
		return;
	}

	channels[channel].data_index = 0;
	channels[channel].packet = *packet;

	uint32_t dpid;
	if (packet->endpoint_type == USBH_ENDPOINT_TYPE_CONTROL) {
		if (packet->control_type == USBH_CONTROL_TYPE_DATA) {
			// For Status OUT stage, Length==0, Status Out PID = 1 */
			if(!packet->datalen) {
				packet->toggle[0] = 1;
			}
			dpid = packet->toggle[0] ? OTG_HCTSIZ_DPID_DATA1 : OTG_HCTSIZ_DPID_DATA0;
		} else {
			dpid = OTG_HCTSIZ_DPID_MDATA;
			packet->toggle[0] = 0;
		}
	} else if(packet->endpoint_type == USBH_ENDPOINT_TYPE_INTERRUPT) {
		dpid = packet->toggle[0] ? OTG_HCTSIZ_DPID_DATA1 : OTG_HCTSIZ_DPID_DATA0;
	} else if (packet->endpoint_type == USBH_ENDPOINT_TYPE_BULK) {
		dpid = packet->toggle[0] ? OTG_HCTSIZ_DPID_DATA1 : OTG_HCTSIZ_DPID_DATA0;
	} else {
		dpid = OTG_HCTSIZ_DPID_DATA0; // ! TODO: BUG
		ERROR("");
	}

	uint32_t num_packets;
	if (packet->datalen) {
		num_packets = ((packet->datalen - 1) / packet->endpoint_size_max) + 1;
	} else {
		num_packets = 1;
	}
	REBASE_CH(OTG_HCTSIZ, channel) = dpid | (num_packets << 19) | packet->datalen;

	stm32f4_usbh_port_channel_setup(dev, channel, OTG_HCCHAR_EPDIR_OUT);

	if (packet->endpoint_type == USBH_ENDPOINT_TYPE_CONTROL ||
		packet->endpoint_type == USBH_ENDPOINT_TYPE_BULK) {

		volatile uint32_t *fifo = &REBASE_CH(OTG_FIFO, channel) + RX_FIFO_SIZE;
		const uint32_t * buf32 = packet->data.out;
		int i;
		LOG_PRINTF("\nSending[%d]: ", packet->datalen);
		for(i = packet->datalen; i >= 4; i-=4) {
#ifdef USART_DEBUG
			const uint8_t *buf8 = (const uint8_t *)buf32;
			LOG_PRINTF("%02X %02X %02X %02X, ", buf8[0], buf8[1], buf8[2], buf8[3]);
#endif
			*fifo++ = *buf32++;

		}

		if (i > 0) {
			*fifo = *buf32&((1 << (8*i)) - 1);
#if USART_DEBUG
			uint8_t *buf8 = (uint8_t *)buf32;
			while (i--) {
				LOG_PRINTF("%02X ", *buf8++);
			}
#endif
		}
		LOG_PRINTF("\n");

	} else {
		volatile uint32_t *fifo = &REBASE_CH(OTG_FIFO, channel) +
			RX_FIFO_SIZE + TX_NP_FIFO_SIZE;
		const uint32_t * buf32 = packet->data.out;
		int i;
		for(i = packet->datalen; i > 0; i-=4) {
			*fifo++ = *buf32++;
		}
	}
	LOG_PRINTF("->WRITE %08X\n", REBASE_CH(OTG_HCCHAR, channel));
}

static void rxflvl_handle(void *drvdata)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	channel_t *channels = dev->channels;
	uint32_t rxstsp = REBASE(OTG_GRXSTSP);
	uint8_t channel = rxstsp&0xf;
	uint32_t len = (rxstsp>>4) & 0x1ff;
	if ((rxstsp&OTG_GRXSTSP_PKTSTS_MASK) == OTG_GRXSTSP_PKTSTS_IN) {
		uint8_t *data = channels[channel].packet.data.in;
		uint32_t *buf32 = (uint32_t *)&data[channels[channel].data_index];

		int32_t i;
		uint32_t extra;
		if (!len) {
			return;
		}
		// Receive data from fifo
		volatile uint32_t *fifo = &REBASE_CH(OTG_FIFO, channel);
		for (i = len; i > 4; i -= 4) {
			*buf32++ = *fifo++;
		}
		extra = *fifo;

		memcpy(buf32, &extra, i);
		channels[channel].data_index += len;

		// If transfer not complete, Enable channel to continue
		if ( channels[channel].data_index < channels[channel].packet.datalen) {
			if (len == channels[channel].packet.endpoint_size_max) {
				REBASE_CH(OTG_HCCHAR, channel) |= OTG_HCCHAR_CHENA;
				LOG_PRINTF("CHENA[%d/%d] ", channels[channel].data_index, channels[channel].packet.datalen);
			}

		}

	} else if ((rxstsp&OTG_GRXSTSP_PKTSTS_MASK) == OTG_GRXSTSP_PKTSTS_IN_COMP) {
#ifdef USART_DEBUG
		uint32_t i;
		LOG_PRINTF("\nDATA: ");
		for (i = 0; i < channels[channel].data_index; i++) {
			uint8_t *data = channels[channel].packet.data.in;
			LOG_PRINTF("%02X ", data[i]);
		}
#endif
	} else if ((rxstsp&OTG_GRXSTSP_PKTSTS_MASK) == OTG_GRXSTSP_PKTSTS_CHH) {

	} else {

	}
}


static enum USBH_POLL_STATUS poll_run(usbh_lld_stm32f4_driver_data_t *dev)
{
	channel_t *channels = dev->channels;

	if (dev->dpstate == DEVICE_POLL_STATE_DISCONN) {
		REBASE(OTG_GINTSTS) = REBASE(OTG_GINTSTS);
		// Check for connection of device
		if ((REBASE(OTG_HPRT) & OTG_HPRT_PCDET)  &&
			(REBASE(OTG_HPRT) & OTG_HPRT_PCSTS) ) {

			dev->dpstate = DEVICE_POLL_STATE_DEVCONN;
			dev->timestamp_us = dev->time_curr_us;
			return USBH_POLL_STATUS_NONE;
		}
	}

	if (dev->dpstate == DEVICE_POLL_STATE_DEVCONN) {
		// May be other condition, e.g. Debounce done,
		// using 0.5s wait by default
		if (dev->time_curr_us - dev->timestamp_us < 500000) {
			return USBH_POLL_STATUS_NONE;
		}

		if ((REBASE(OTG_HPRT) & OTG_HPRT_PCDET)  &&
			(REBASE(OTG_HPRT) & OTG_HPRT_PCSTS) ) {
			if ((REBASE(OTG_HPRT) & OTG_HPRT_PSPD_MASK) == OTG_HPRT_PSPD_FULL) {
				REBASE(OTG_HFIR) = (REBASE(OTG_HFIR) & ~OTG_HFIR_FRIVL_MASK) | 48000;
				if ((REBASE(OTG_HCFG) & OTG_HCFG_FSLSPCS_MASK) != OTG_HCFG_FSLSPCS_48MHz) {
					REBASE(OTG_HCFG) = (REBASE(OTG_HCFG) & ~OTG_HCFG_FSLSPCS_MASK) | OTG_HCFG_FSLSPCS_48MHz;
					LOG_PRINTF("\n Reset Full-Speed \n");
				}
				channels_init(dev);
				dev->dpstate = DEVICE_POLL_STATE_DEVRST;
				reset_start(dev);

			} else if ((REBASE(OTG_HPRT) & OTG_HPRT_PSPD_MASK) == OTG_HPRT_PSPD_LOW) {
				REBASE(OTG_HFIR) = (REBASE(OTG_HFIR) & ~OTG_HFIR_FRIVL_MASK) | 6000;
				if ((REBASE(OTG_HCFG) & OTG_HCFG_FSLSPCS_MASK) != OTG_HCFG_FSLSPCS_6MHz) {
					REBASE(OTG_HCFG) = (REBASE(OTG_HCFG) & ~OTG_HCFG_FSLSPCS_MASK) | OTG_HCFG_FSLSPCS_6MHz;
					LOG_PRINTF("\n Reset Low-Speed \n");
				}

				channels_init(dev);
				dev->dpstate = DEVICE_POLL_STATE_DEVRST;
				reset_start(dev);
			}
			return USBH_POLL_STATUS_NONE;
		}
	}

	if (dev->dpstate == DEVICE_POLL_STATE_DEVRST) {
		if (dev->time_curr_us - dev->timestamp_us < 210000) {
			return USBH_POLL_STATUS_NONE;
		} else {
			dev->dpstate = DEVICE_POLL_STATE_RUN;
		}
	}

	// ELSE RUN

	if (REBASE(OTG_GINTSTS) & OTG_GINTSTS_SOF) {
		REBASE(OTG_GINTSTS) = OTG_GINTSTS_SOF;
	}

	while (REBASE(OTG_GINTSTS) & OTG_GINTSTS_RXFLVL) {
		//receive data
		rxflvl_handle(dev);
	}

	if (REBASE(OTG_GINTSTS) & OTG_GINTSTS_HPRTINT) {
		if (REBASE(OTG_HPRT) & OTG_HPRT_PENCHNG) {
			uint32_t hprt = REBASE(OTG_HPRT);
			// Clear Interrupt
			// HARDWARE BUG - not mentioned in errata
			// To clear interrupt write 0 to PENA
			// To disable port write 1 to PENCHNG
			REBASE(OTG_HPRT) &= ~OTG_HPRT_PENA;
			LOG_PRINTF("PENCHNG");
			if ((hprt & OTG_HPRT_PENA)) {
				return USBH_POLL_STATUS_DEVICE_CONNECTED;
			}

		}

		if (REBASE(OTG_HPRT) & OTG_HPRT_POCCHNG) {
			// TODO: Check for functionality
			REBASE(OTG_HPRT) |= OTG_HPRT_POCCHNG;
			LOG_PRINTF("POCCHNG");
		}
	}

	if (REBASE(OTG_GINTSTS) & OTG_GINTSTS_DISCINT) {
		REBASE(OTG_GINTSTS) = OTG_GINTSTS_DISCINT;
		LOG_PRINTF("DISCINT");

		/*
		 * When the voltage drops, DISCINT interrupt is generated although
		 * Device is connected, so there is no need to reinitialize channels.
		 * Often, DISCINT is bad interpreted upon insertion of device
		 */
		if (!(REBASE(OTG_HPRT) & OTG_HPRT_PCSTS)) {
			LOG_PRINTF("discint processsing...");
			channels_init(dev);
		}
		REBASE(OTG_GINTSTS) = REBASE(OTG_GINTSTS);
		dev->dpstate = DEVICE_POLL_STATE_DISCONN;
		return USBH_POLL_STATUS_DEVICE_DISCONNECTED;
	}

	if (REBASE(OTG_GINTSTS) & OTG_GINTSTS_HCINT) {
		uint32_t channel;

		for(channel = 0; channel < dev->num_channels; channel++)
		{
			if (channels[channel].state != CHANNEL_STATE_WORK ||
				!(REBASE(OTG_HAINT)&(1<<channel))) {
				continue;
			}
			uint32_t hcint = REBASE_CH(OTG_HCINT, channel);
			uint8_t eptyp = channels[channel].packet.endpoint_type;

			// Write
			if (!(REBASE_CH(OTG_HCCHAR, channel)&OTG_HCCHAR_EPDIR_IN)) {

				if (hcint & OTG_HCINT_NAK) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_NAK;
					LOG_PRINTF("NAK\n");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EAGAIN;
					cb_data.transferred_length = channels[channel].data_index;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);

				}

				if (hcint & OTG_HCINT_ACK) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_ACK;
					LOG_PRINTF("ACK");
					if (eptyp == USBH_ENDPOINT_TYPE_CONTROL) {
						channels[channel].packet.toggle[0] = 1;
					} else {
						channels[channel].packet.toggle[0] ^= 1;
					}
				}

				if (hcint & OTG_HCINT_XFRC) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_XFRC;
					LOG_PRINTF("XFRC\n");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_OK;
					cb_data.transferred_length = channels[channel].data_index;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);
					continue;
				}

				if (hcint & OTG_HCINT_FRMOR) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_FRMOR;
					LOG_PRINTF("FRMOR");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
					cb_data.transferred_length = 0;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);
				}

				if (hcint & OTG_HCINT_TXERR) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_TXERR;
					LOG_PRINTF("TXERR");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EAGAIN;
					cb_data.transferred_length = 0;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);


				}

				if (hcint & OTG_HCINT_STALL) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_STALL;
					LOG_PRINTF("STALL");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
					cb_data.transferred_length = 0;


					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);
				}

				if (hcint & OTG_HCINT_CHH) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_CHH;
					LOG_PRINTF("CHH");

					free_channel(dev, channel);
				}
			} else { // Read

				if (hcint & OTG_HCINT_DTERR) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_DTERR;
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_NAK;
					LOG_PRINTF("DTERR");
					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
					cb_data.transferred_length = 0;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);
				}

				if (hcint & OTG_HCINT_NAK) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_NAK;
					if (eptyp == USBH_ENDPOINT_TYPE_CONTROL) {
						 LOG_PRINTF("NAK");
					}

					REBASE_CH(OTG_HCCHAR, channel) |= OTG_HCCHAR_CHENA;

				}


				if (hcint & OTG_HCINT_ACK) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_ACK;
					LOG_PRINTF("ACK");

					channels[channel].packet.toggle[0] ^= 1;

				}



				if (hcint & OTG_HCINT_XFRC) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_XFRC;
					LOG_PRINTF("XFRC\n");

					free_channel(dev, channel);
					usbh_packet_callback_data_t cb_data;
					if (channels[channel].data_index == channels[channel].packet.datalen) {
						cb_data.status = USBH_PACKET_CALLBACK_STATUS_OK;
					} else {
						cb_data.status = USBH_PACKET_CALLBACK_STATUS_ERRSIZ;
					}
					cb_data.transferred_length = channels[channel].data_index;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);

					continue;
				}

				if (hcint & OTG_HCINT_BBERR) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_BBERR;
					LOG_PRINTF("BBERR");
					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
					cb_data.transferred_length = 0;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);
				}

				if (hcint & OTG_HCINT_FRMOR) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_FRMOR;
					LOG_PRINTF("FRMOR");

				}

				if (hcint & OTG_HCINT_TXERR) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_TXERR;
					LOG_PRINTF("TXERR");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
					cb_data.transferred_length = 0;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);

				}

				if (hcint & OTG_HCINT_STALL) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_STALL;
					LOG_PRINTF("STALL");

					free_channel(dev, channel);

					usbh_packet_callback_data_t cb_data;
					cb_data.status = USBH_PACKET_CALLBACK_STATUS_EFATAL;
					cb_data.transferred_length = 0;

					channels[channel].packet.callback(
						channels[channel].packet.callback_arg,
						cb_data);

				}
				if (hcint & OTG_HCINT_CHH) {
					REBASE_CH(OTG_HCINT, channel) = OTG_HCINT_CHH;
					LOG_PRINTF("CHH");
					free_channel(dev, channel);
				}

			}
		}
	}

	if (REBASE(OTG_GINTSTS) & OTG_GINTSTS_MMIS) {
		REBASE(OTG_GINTSTS) = OTG_GINTSTS_MMIS;
		LOG_PRINTF("Mode mismatch");
	}

	if (REBASE(OTG_GINTSTS) & OTG_GINTSTS_IPXFR) {
		REBASE(OTG_GINTSTS) = OTG_GINTSTS_IPXFR;
		LOG_PRINTF("IPXFR");
	}

	return USBH_POLL_STATUS_NONE;
}

/*
 * Sequence numbers are hardcoded, since it is used
 * locally in poll_init() function.
 * If value of poll_sequence is needed elsewhere, enum must be defined.
 *
 */
static void poll_init(usbh_lld_stm32f4_driver_data_t *dev)
{
	//=======================================

	int done = 0;
	/* Wait for AHB idle. */
	switch (dev->poll_sequence) {
	case 0:// wait until AHBIDL is set
		if (REBASE(OTG_GRSTCTL) & OTG_GRSTCTL_AHBIDL) {
			done = 1;
		}
		break;

	case 1:// wait 1ms and issue core soft reset

		// needs delay to not hang?? Do not know why.
		// Maybe after AHBIDL is set, it needs to set up some things
		if (dev->time_curr_us - dev->timestamp_us > 1000) {
			REBASE(OTG_GRSTCTL) |= OTG_GRSTCTL_CSRST;
			done = 1;
		}
		break;

	case 2:// wait until core soft reset processing is done
		if (!(REBASE(OTG_GRSTCTL) & OTG_GRSTCTL_CSRST)) {
			done = 1;
		}
		break;

	case 3:// wait for 50ms
		if (dev->time_curr_us - dev->timestamp_us > 50000) {
			done = 1;
		}
		break;

	case 4:// wait until AHBIDL is set and power up the USB
		if (REBASE(OTG_GRSTCTL) & OTG_GRSTCTL_AHBIDL) {
			REBASE(OTG_GCCFG) = OTG_GCCFG_VBUSASEN | OTG_GCCFG_VBUSBSEN |
					OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN;
			done = 1;
		}
		break;

	case 5:// wait for 50ms and force host only mode
		if (dev->time_curr_us - dev->timestamp_us > 50000) {

			// Core initialized
			// Force host only mode.
			REBASE(OTG_GUSBCFG) |= OTG_GUSBCFG_FHMOD;
			done = 1;
		}
		break;

	case 6:// wait for 200ms and reset PHY clock start reset processing
		if (dev->time_curr_us - dev->timestamp_us > 200000) {
			/* Restart the PHY clock. */
			REBASE(OTG_PCGCCTL) = 0;

			REBASE(OTG_HCFG) = 	(REBASE(OTG_HCFG) & ~OTG_HCFG_FSLSPCS_MASK) |
							OTG_HCFG_FSLSPCS_48MHz;

			// Start reset processing
			REBASE(OTG_HPRT) |= OTG_HPRT_PRST;

			done = 1;

		}
		break;

	case 7:// wait for reset processing to be done(12ms), disable PRST
		if (dev->time_curr_us - dev->timestamp_us > 12000) {

			REBASE(OTG_HPRT) &= ~OTG_HPRT_PRST;
			done = 1;
		}
		break;

	case 8:// wait 12ms after PRST was disabled, configure fifo
		if (dev->time_curr_us - dev->timestamp_us > 12000) {

			REBASE(OTG_HCFG) &= ~OTG_HCFG_FSLSS;

			REBASE(OTG_GRXFSIZ) = RX_FIFO_SIZE;
			REBASE(OTG_GNPTXFSIZ) = (TX_NP_FIFO_SIZE << 16) |
								RX_FIFO_SIZE;
			REBASE(OTG_HPTXFSIZ) = (TX_P_FIFO_SIZE << 16) |
								(RX_FIFO_SIZE +	TX_NP_FIFO_SIZE);

			// FLUSH RX FIFO
			REBASE(OTG_GRSTCTL) |= OTG_GRSTCTL_RXFFLSH;

			done = 1;
		}
		break;

	case 9: // wait to RX FIFO become flushed, flush TX
		if (!(REBASE(OTG_GRSTCTL) & OTG_GRSTCTL_RXFFLSH)) {
			REBASE(OTG_GRSTCTL) |= OTG_GRSTCTL_TXFFLSH | (0x10 << 6);

			done = 1;
		}
		break;

	case 10: // wait to TX FIFO become flushed
		if (!(REBASE(OTG_GRSTCTL) & OTG_GRSTCTL_TXFFLSH)) {

			channels_init(dev);

			REBASE(OTG_GOTGINT) |= 1 << 19;
			REBASE(OTG_GINTMSK) = 0;
			REBASE(OTG_GINTSTS) = ~0;
			REBASE(OTG_HPRT) |= OTG_HPRT_PPWR;

			done = 1;
		}
		break;

	case 11: // wait 200ms
		if (dev->time_curr_us - dev->timestamp_us > 200000) {

			// Uncomment to enable Interrupt generation
			REBASE(OTG_GAHBCFG) |= OTG_GAHBCFG_GINT;

			LOG_PRINTF("INIT COMPLETE\n");

			// Finish
			dev->state = DEVICE_STATE_RUN;
			dev->dpstate = DEVICE_POLL_STATE_DISCONN;

			done = 1;
		}
	}

	if (done) {
		dev->poll_sequence++;
		dev->timestamp_us = dev->time_curr_us;
		LOG_PRINTF("\t\t POLL SEQUENCE %d\n", dev->poll_sequence);
	}

}

static void poll_reset(usbh_lld_stm32f4_driver_data_t *dev)
{
	if (dev->time_curr_us - dev->timestamp_us > 10000) {
		REBASE(OTG_HPRT) &= ~OTG_HPRT_PRST;
		dev->state = dev->state_prev;
		dev->state_prev = DEVICE_STATE_RESET;

		LOG_PRINTF("RESET");
	} else {
		LOG_PRINTF("waiting %d < %d\n",dev->time_curr_us, dev->timestamp_us);
	}
}

static enum USBH_POLL_STATUS poll(void *drvdata, uint32_t time_curr_us)
{
	(void)time_curr_us;

	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	enum USBH_POLL_STATUS ret = USBH_POLL_STATUS_NONE;

	dev->time_curr_us = time_curr_us;

	switch (dev->state) {
	case DEVICE_STATE_RUN:
		ret = poll_run(dev);
		break;

	case DEVICE_STATE_INIT:
		poll_init(dev);
		break;

	case DEVICE_STATE_RESET:
		poll_reset(dev);
		break;

	default:
		break;
	}

	return ret;

}


/**
 *
 * Returns positive free channel id
 * 	otherwise -1 for error
 */
static int8_t get_free_channel(void *drvdata)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	channel_t *channels = dev->channels;
	uint32_t i = 0;
	for (i = 0; i < dev->num_channels; i++) {
		if (dev->channels[i].state == CHANNEL_STATE_FREE &&
			!(REBASE_CH(OTG_HCCHAR, i) & OTG_HCCHAR_CHENA)) {
			channels[i].state = CHANNEL_STATE_WORK;
			REBASE_CH(OTG_HCINT, i) = ~0;
			REBASE_CH(OTG_HCINTMSK, i) |= OTG_HCINTMSK_ACKM | OTG_HCINTMSK_NAKM |
				OTG_HCINTMSK_TXERRM | OTG_HCINTMSK_XFRCM |
				OTG_HCINTMSK_DTERRM | OTG_HCINTMSK_BBERRM |
				OTG_HCINTMSK_CHHM | OTG_HCINTMSK_STALLM |
				OTG_HCINTMSK_FRMORM;
			REBASE(OTG_HAINTMSK) |= (1 << i);
			return i;
		}
	}
	return -1;
}

/*
 * Do not clear callback and callback data, so channel can be freed even before callback is called
 * This saves number of active channels: When one transfer ends, in callback driver can write/read to this channel again (indirectly)
 */
static void free_channel(void *drvdata, uint8_t channel)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	channel_t *channels = dev->channels;

	if (REBASE_CH(OTG_HCCHAR, channel) & OTG_HCCHAR_CHENA) {
		REBASE_CH(OTG_HCCHAR, channel) |= OTG_HCCHAR_CHDIS;
		REBASE_CH(OTG_HCINT, channel) = ~0;
		LOG_PRINTF("\nDisabling channel %d\n", channel);
	} else {
		channels[channel].state = CHANNEL_STATE_FREE;
	}
}
/**
 * Init channels
 */
static void channels_init(void *drvdata)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;

	uint32_t i = 0;
	for (i = 0; i < dev->num_channels; i++) {
		REBASE_CH(OTG_HCINT, i) = ~0;
		REBASE_CH(OTG_HCINTMSK, i) = 0x7ff;
		free_channel(dev, i);
	}

	// Enable interrupt mask bits for all channels
	REBASE(OTG_HAINTMSK) = (1 << dev->num_channels) - 1;
}

/**
 * Get speed of connected device
 *
 */
static enum USBH_SPEED root_speed(void *drvdata)
{
	usbh_lld_stm32f4_driver_data_t *dev = drvdata;
	(void)dev;
	uint32_t hprt_speed = REBASE(OTG_HPRT) & OTG_HPRT_PSPD_MASK;
	if (hprt_speed == OTG_HPRT_PSPD_LOW) {
		return USBH_SPEED_LOW;
	} else if(hprt_speed == OTG_HPRT_PSPD_FULL) {
		return USBH_SPEED_FULL;
	} else if(hprt_speed == OTG_HPRT_PSPD_HIGH) {
		return USBH_SPEED_HIGH;
	} else {
		// Should not happen(let the compiler be happy)
		return USBH_SPEED_FULL;
	}
}
#endif // if defined otg_hs or otg_fs


#ifdef USART_DEBUG

/**
 * Just for debug
 */
void print_channels(const void *lld)
{
	usbh_lld_stm32f4_driver_data_t *dev = ((usbh_low_level_driver_t *)lld)->driver_data;
	channel_t *channels = dev->channels;
	int32_t i;
	LOG_PRINTF("\nCHANNELS: \n");
	for (i = 0;i < dev->num_channels;i++) {
		LOG_PRINTF("%4d %4d %4d %08X\n", channels[i].state, channels[i].packet.address, channels[i].packet.datalen, MMIO32(dev->base + OTG_HCINT(i)));
	}
}
#endif

// USB Full Speed - OTG_FS
#if defined(USE_STM32F4_USBH_DRIVER_FS)
#define NUM_CHANNELS_FS		(8)
static channel_t channels_fs[NUM_CHANNELS_FS];
static usbh_lld_stm32f4_driver_data_t driver_data_fs = {
	.base = USB_OTG_FS_BASE,
	.channels = channels_fs,
	.num_channels = NUM_CHANNELS_FS
};
const usbh_low_level_driver_t usbh_lld_stm32f4_driver_fs = {
	.init = init,
	.poll = poll,
	.read = read,
	.write = write,
	.root_speed = root_speed,
	.driver_data = &driver_data_fs
};
#endif

// USB High Speed - OTG_HS
#if defined(USE_STM32F4_USBH_DRIVER_HS)
#define NUM_CHANNELS_HS		(12)
static channel_t channels_hs[NUM_CHANNELS_HS];
static usbh_lld_stm32f4_driver_data_t driver_data_hs = {
	.base = USB_OTG_HS_BASE,
	.channels = channels_hs,
	.num_channels = NUM_CHANNELS_HS
};

const usbh_low_level_driver_t usbh_lld_stm32f4_driver_hs = {
	.init = init,
	.poll = poll,
	.read = read,
	.write = write,
	.root_speed = root_speed,
	.driver_data = &driver_data_hs
};

#endif
