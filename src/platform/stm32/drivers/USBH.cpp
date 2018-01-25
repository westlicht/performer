#include "USBH.h"
#include "HighResolutionTimer.h"

#include "SystemConfig.h"

#include "core/Debug.h"
#include "core/midi/MIDIMessage.h"

#include "usbh_core.h"				/// provides usbh_init() and usbh_poll()
#include "usbh_lld_stm32f4.h"		/// provides low level usb host driver for stm32f4 platform
// #include "usbh_driver_hid.h"		/// provides generic usb device driver for Human Interface Device (HID)
#include "usbh_driver_hub.h"		/// provides usb full speed hub driver (Low speed devices on hub are not supported)
#include "usbh_driver_ac_midi.h"	/// provides usb device driver for midi class devices

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/otg_hs.h>
#include <libopencm3/stm32/otg_fs.h>

#define USB_PWR_EN_PORT GPIOC
#define USB_PWR_EN_PIN GPIO9
#define USB_PWR_FAULT_PORT GPIOA
#define USB_PWR_FAULT_PIN GPIO8

static USBH *g_usbh;

static const usbh_dev_driver_t *device_drivers[] = {
	&usbh_hub_driver,
	// &usbh_hid_driver,
	&usbh_midi_driver,
	nullptr
};

static const usbh_low_level_driver_t * const lld_drivers[] = {
#ifdef USE_STM32F4_USBH_DRIVER_FS
	&usbh_lld_stm32f4_driver_fs, // Make sure USE_STM32F4_USBH_DRIVER_FS is defined in usbh_config.h
#endif

#ifdef USE_STM32F4_USBH_DRIVER_HS
	&usbh_lld_stm32f4_driver_hs, // Make sure USE_STM32F4_USBH_DRIVER_HS is defined in usbh_config.h
#endif
	nullptr
};

// static void hid_in_message_handler(uint8_t device_id, const uint8_t *data, uint32_t length)
// {
// 	(void)device_id;
// 	(void)data;
// 	if (length < 4) {
// 		DBG("data too short, type=%d", hid_get_type(device_id));
// 		return;
// 	}

// 	// print only first 4 bytes, since every mouse should have at least these four set.
// 	// Report descriptors are not read by driver for now, so we do not know what each byte means
// 	// DBG("HID EVENT %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
// 	if (hid_get_type(device_id) == HID_TYPE_KEYBOARD) {
// 		static int x = 0;
// 		if (x != data[2]) {
// 			x = data[2];
// 			hid_set_report(device_id, x);
// 		}
// 	}
//     if (hid_get_type(device_id) == HID_TYPE_MOUSE) {
//         static int x, y;
//         x += (int8_t) data[1];
//         y += (int8_t) data[2];
//         g_usbh->setMousePos(x, y);
//         DBG("mouse x = %d, y = %d", x, y);
//     }
// }

// static const hid_config_t hid_config = {
// 	.hid_in_message_handler = &hid_in_message_handler
// };

struct MIDIDriverHandler {

    static void connectHandler(int device) {
        DBG("MIDI device connected (id=%d)", device);
        g_usbh->midiConnectDevice(device);
    }

    static void disconnectHandler(int device) {
        DBG("MIDI device disconnected (id=%d)", device);
        g_usbh->midiDisconnectDevice(device);
    }

    static void recvHandler(int device_id, uint8_t *data) {
        // uint8_t cable = data[0] >> 4;
        uint8_t code = data[0] & 0xf;
        MIDIMessage message;

        switch (code) {
        case 0x0: // (1, 2 or 3 bytes) Miscellaneous function codes. Reserved for future extensions.
        case 0x1: // (1, 2 or 3 bytes) Cable events. Reserved for future expansion.
        case 0x4: // (3 bytes) SysEx starts or continues
        case 0x6: // (2 bytes) SysEx ends with following two bytes.
        case 0x7: // (3 bytes) SysEx ends with following three bytes.
            // ignore for now
            return;
        case 0x5: // (1 bytes) Single-byte System Common Message or SysEx ends with following single byte.
            message = MIDIMessage(data[1]);
            g_usbh->midiEnqueueMessage(device_id, message);
            break;
        case 0x2: // (2 bytes) Two-byte System Common messages like MTC, SongSelect, etc.
        case 0xC: // (2 bytes) Program Change
        case 0xD: // (2 bytes) Channel Pressure
            message = MIDIMessage(data[1], data[2]);
            g_usbh->midiEnqueueMessage(device_id, message);
            break;
        case 0x3: // (3 bytes) Three-byte System Common messages like SPP, etc.
        case 0x8: // (3 bytes) Note-off
        case 0x9: // (3 bytes) Note-on
        case 0xA: // (3 bytes) Poly-KeyPress
        case 0xB: // (3 bytes) Control Change
        case 0xE: // (3 bytes) PitchBend Change
            message = MIDIMessage(data[1], data[2], data[3]);
            g_usbh->midiEnqueueMessage(device_id, message);
            break;
        case 0xF: // (1 bytes) Single Byte
            g_usbh->midiEnqueueData(device_id, data[1]);
            return;
        }
    }

    static void write(uint8_t device, MIDIMessage &message) {
        uint8_t data[4];

        data[0] = message.status() >> 4;
        data[1] = message.status();
        data[2] = message.data0();
        data[3] = message.data1();

        usbh_midi_write(device, data, 4, &writeCallback);
    }

    static void writeCallback(uint8_t bytes_written) {
        uint8_t device;
        MIDIMessage message;
        if (g_usbh->midiDequeueMessage(&device, &message)) {
            if (g_usbh->midiDeviceConnected(device)) {
                write(device, message);
            }
        }
    }
};

static const midi_config_t midi_config = {
    .read_callback = &MIDIDriverHandler::recvHandler,
    .notify_connected = &MIDIDriverHandler::connectHandler,
    .notify_disconnected = &MIDIDriverHandler::disconnectHandler,
};




USBH::USBH(USBMIDI &usbMidi) :
    _usbMidi(usbMidi)
{}

void USBH::init() {
    g_usbh = this;

	rcc_periph_clock_enable(RCC_GPIOA); // OTG_FS + USB_PWR_FAULT
	rcc_periph_clock_enable(RCC_GPIOC); // USB_PWR_EN

    // USB_PWR_EN
	gpio_mode_setup(USB_PWR_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, USB_PWR_EN_PIN);
    gpio_clear(USB_PWR_EN_PORT, USB_PWR_EN_PIN);

    // USB_PWR_FAULT
	gpio_mode_setup(USB_PWR_FAULT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, USB_PWR_FAULT_PIN);



	// rcc_periph_clock_enable(RCC_GPIOB); // OTG_HS
	// rcc_periph_clock_enable(RCC_GPIOG); // USB OTG Power Up
	// rcc_periph_clock_enable(RCC_GPIOC); // USART + OTG_FS charge pump
	// rcc_periph_clock_enable(RCC_GPIOD); // LEDS

	// periphery
	// rcc_periph_clock_enable(RCC_USART6); // USART
	rcc_periph_clock_enable(RCC_OTGFS); // OTG_FS
	// rcc_periph_clock_enable(RCC_OTGHS); // OTG_HS
	// rcc_periph_clock_enable(RCC_TIM6); // TIM6

    // gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
    // gpio_set(GPIOG, GPIO6);


	// /* Set GPIO12-15 (in GPIO port D) to 'output push-pull'. */
	// gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
	// 		GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	/* Set	 */
	// gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
	// gpio_clear(GPIOC, GPIO0);

	// gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
	// 		GPIO13 | GPIO14 | GPIO15);
	// gpio_set_af(GPIOB, GPIO_AF12, GPIO13 | GPIO14 | GPIO15);


	// OTG_FS
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO11 | GPIO12);

	// OTG_HS
	// gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15 | GPIO14);
	// gpio_set_af(GPIOB, GPIO_AF12, GPIO14 | GPIO15);

	// hid_driver_init(&hid_config);
	hub_driver_init();
	midi_driver_init(&midi_config);


	// gpio_set(GPIOD,  GPIO13);
	usbh_init(lld_drivers, device_drivers);
	// gpio_clear(GPIOD,  GPIO13);
}

void USBH::process() {
    uint32_t time_us = HighResolutionTimer::us();

    usbh_poll(time_us);

    // Start sending MIDI messages
    uint8_t device;
    MIDIMessage message;
    if (midiDequeueMessage(&device, &message)) {
        if (midiDeviceConnected(device)) {
            MIDIDriverHandler::write(device, message);
        }
    }
}

void USBH::powerOn() {
    gpio_set(USB_PWR_EN_PORT, USB_PWR_EN_PIN);
}

void USBH::powerOff() {
    gpio_clear(USB_PWR_EN_PORT, USB_PWR_EN_PIN);
}

bool USBH::powerFault() {
    return !gpio_get(USB_PWR_FAULT_PORT, USB_PWR_FAULT_PIN);
}
