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

#ifndef USBH_DEVICE_DRIVER_
#define USBH_DEVICE_DRIVER_

#include "usbh_config.h"
#include "usbh_core.h"

#include <libopencm3/usb/usbstd.h>
#include <stdint.h>

BEGIN_DECLS

enum USBH_ENDPOINT_TYPE {
	USBH_ENDPOINT_TYPE_CONTROL = 0,
	USBH_ENDPOINT_TYPE_ISOCHRONOUS = 1,
	USBH_ENDPOINT_TYPE_BULK = 2,
	USBH_ENDPOINT_TYPE_INTERRUPT = 3,
};

enum USBH_SPEED {
	USBH_SPEED_FULL = 0,
	USBH_SPEED_LOW = 1,
	USBH_SPEED_HIGH = 2,
};

enum USBH_PACKET_CALLBACK_STATUS {
	USBH_PACKET_CALLBACK_STATUS_OK = 0,
	USBH_PACKET_CALLBACK_STATUS_ERRSIZ = 1,
	USBH_PACKET_CALLBACK_STATUS_EAGAIN = 2, // -- TODO: automatic handling of transmit errors 3xTXERR->FATAL
	USBH_PACKET_CALLBACK_STATUS_EFATAL = 3
};

enum USBH_POLL_STATUS {
	USBH_POLL_STATUS_NONE,
	USBH_POLL_STATUS_DEVICE_CONNECTED,
	USBH_POLL_STATUS_DEVICE_DISCONNECTED
};

enum USBH_CONTROL_TYPE {
	USBH_CONTROL_TYPE_SETUP,
	USBH_CONTROL_TYPE_DATA
};

enum USBH_ENUM_STATE {
	USBH_ENUM_STATE_SET_ADDRESS,
	USBH_ENUM_STATE_FIRST = USBH_ENUM_STATE_SET_ADDRESS,
	USBH_ENUM_STATE_DEVICE_DT_READ_SHORT_SETUP,
	USBH_ENUM_STATE_DEVICE_DT_READ_SHORT_COMPLETE,
	USBH_ENUM_STATE_DEVICE_DT_READ_SETUP,
	USBH_ENUM_STATE_DEVICE_DT_READ_COMPLETE,
	USBH_ENUM_STATE_CONFIGURATION_DT_HEADER_READ_SETUP,
	USBH_ENUM_STATE_CONFIGURATION_DT_HEADER_READ_COMPLETE,
	USBH_ENUM_STATE_CONFIGURATION_DT_READ_SETUP,
	USBH_ENUM_STATE_CONFIGURATION_DT_READ_COMPLETE,
	USBH_ENUM_STATE_SET_CONFIGURATION_SETUP,
	USBH_ENUM_STATE_SET_CONFIGURATION_COMPLETE,
	USBH_ENUM_STATE_FIND_DRIVER,
};

enum USBH_CONTROL_STATE {
	USBH_CONTROL_STATE_NONE,
	USBH_CONTROL_STATE_SETUP,
	USBH_CONTROL_STATE_DATA,
	USBH_CONTROL_STATE_STATUS,
};

typedef struct _usbh_device usbh_device_t;

struct _usbh_packet_callback_data {
	/// status - it is used for reporting of the errors
	enum USBH_PACKET_CALLBACK_STATUS status;

	/// count of bytes that has been actually transferred
	uint32_t transferred_length;
};
typedef struct _usbh_packet_callback_data usbh_packet_callback_data_t;

typedef void (*usbh_packet_callback_t)(usbh_device_t *dev, usbh_packet_callback_data_t status);

struct _usbh_control {
	enum USBH_CONTROL_STATE state;
	usbh_packet_callback_t callback;
	union {
		const void *out;
		void *in;
	} data;
	uint16_t data_length;
	struct usb_setup_data setup_data;
};
typedef struct _usbh_control usbh_control_t;

/**
 * @brief The _usbh_device struct
 *
 * This represents exactly one connected device.
 */
struct _usbh_device {
	/// max packet size for control endpoint(0)
	uint16_t packet_size_max0;

	/// Device's address
	int8_t address;

	/// @see USBH_SPEED
	enum USBH_SPEED speed;

	/// state used for enumeration purposes
	enum USBH_ENUM_STATE state;
	usbh_control_t control;

	/// toggle bit
	uint8_t toggle0;

	/**
	 * @brief drv - device driver used for this connected device
	 */

	const usbh_dev_driver_t *drv;
	/**
	 * @brief drvdata - device driver's private data
	 */
	void *drvdata;

	/**
	 * @brief lld - pointer to a low-level driver's instance
	 */
	const void *lld;
};
typedef struct _usbh_device usbh_device_t;

struct _usbh_packet {
	/// pointer to data
	union {
		const void *out;
		void *in;
	} data;

	/// length of the data (up to 1023)
	uint16_t datalen;

	/// Device's address
	int8_t address;

	/**
	 * @brief Endpoint type
	 * @see USBH_ENDPOINT_TYPE
	 */
	enum USBH_ENDPOINT_TYPE endpoint_type;

	enum USBH_CONTROL_TYPE control_type;

	/// Endpoint number 0..15
	uint8_t endpoint_address;

	/// Max packet size for an endpoint
	uint16_t endpoint_size_max;

	/// @see USBH_SPEED
	enum USBH_SPEED speed;
	uint8_t *toggle;

	/**
	 * @brief callback this will be called when the packet is finished - either successfuly or not.
	 */
	usbh_packet_callback_t callback;

	/**
	 * @brief callback_arg argument passed into callback
	 *
	 * Low level driver is not allowed to alter the data pointed by callback_arg
	 */
	void *callback_arg;
};
typedef struct _usbh_packet usbh_packet_t;

struct _usbh_low_level_driver {
	/**
	 * @brief init initialization routine of the low-level driver
	 *
	 *
	 * This function is called during the initialization of the library
	 *
	 * @see usbh_init()
	 */
	void (*init)(void *drvdata);

	/**
	 * write - perform a write to a device
	 * @see usbh_packet_t
	 */
	void (*write)(void *drvdata, const usbh_packet_t *packet);

	/**
	 * @brief read - perform a read from a device
	 * @see usbh_packet_t
	 */
	void (*read)(void *drvdata, usbh_packet_t *packet);

	/**
	 * @brief this is called as a part of @ref usbh_poll() routine
	 */
	enum USBH_POLL_STATUS (*poll)(void *drvdata, uint32_t time_curr_us);

	/**
	 * @brief speed of the low-level bus
	 */
	enum USBH_SPEED (*root_speed)(void *drvdata);

	/**
	 * @brief Pointer to Low-level driver data
	 *
	 * Data pointed by this pointer should not be altered by the logic other from low-level driver's logic
	 */
	void *driver_data;
};
typedef struct _usbh_low_level_driver usbh_low_level_driver_t;

struct _usbh_generic_data {
	usbh_device_t usbh_device[USBH_MAX_DEVICES];
	uint8_t usbh_buffer[BUFFER_ONE_BYTES];
};
typedef struct _usbh_generic_data usbh_generic_data_t;

/// set to -1 for unused items ("don't care" functionality) @see find_driver()
struct _usbh_dev_driver_info {
	int32_t deviceClass;
	int32_t deviceSubClass;
	int32_t deviceProtocol;
	int32_t idVendor;
	int32_t idProduct;
	int32_t ifaceClass;
	int32_t ifaceSubClass;
	int32_t ifaceProtocol;
};
typedef struct _usbh_dev_driver_info usbh_dev_driver_info_t;


struct _usbh_dev_driver {
	/**
	 * @brief init is initialization routine of the device driver
	 *
	 * This function is called during the initialization of the device driver
	 */
	void *(*init)(usbh_device_t *usbh_dev, const usbh_dev_driver_info_t * device_info);

	/**
	 * @brief analyze descriptor
	 * @param[in/out] drvdata is the device driver's private data
	 * @param[in] descriptor is the pointer to the descriptor that should
	 *		be parsed in order to prepare driver to be loaded
	 *
	 * @retval true when the enumeration is complete and the driver is ready to be used
	 * @retval false when the device driver is not ready to be used
	 *
	 * This should be used for getting correct endpoint numbers, getting maximum sizes of endpoints.
	 * Should return true, when no more data is needed.
	 *
	 */
	bool (*analyze_descriptor)(void *drvdata, void *descriptor);

	/**
	 * @brief poll method is called periodically by the library core
	 * @param[in/out] drvdata is the device driver's private data
	 * @param[in] time_curr_us current timestamp in microseconds
	 * @see usbh_poll()
	 */
	void (*poll)(void *drvdata, uint32_t time_curr_us);

	/**
	 * @brief unloads the device driver
	 * @param[in/out] drvdata is the device driver's private data
	 *
	 * This should free any data associated with this device
	 */
	void (*remove)(void *drvdata);

	/**
	 * @brief info - compatibility information about the driver. It is used by the core during device enumeration
	 * @see find_driver()
	 */
	const usbh_dev_driver_info_t * const info;
};
typedef struct _usbh_dev_driver usbh_dev_driver_t;

#define ERROR(arg) LOG_PRINTF("UNHANDLED_ERROR %d: file: %s, line: %d",\
							arg, __FILE__, __LINE__)


/* Hub related functions */

usbh_device_t *usbh_get_free_device(const usbh_device_t *dev);
bool usbh_enum_available(void);
void device_enumeration_start(usbh_device_t *dev);

/* All devices functions */
void usbh_read(usbh_device_t *dev, usbh_packet_t *packet);
void usbh_write(usbh_device_t *dev, const usbh_packet_t *packet);

/* Helper functions used by device drivers */
void device_control(usbh_device_t *dev, usbh_packet_callback_t callback, const struct usb_setup_data *setup_data, void *data);
void device_remove(usbh_device_t *dev);

END_DECLS

#endif
