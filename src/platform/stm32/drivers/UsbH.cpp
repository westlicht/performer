#include "UsbH.h"
#include "HighResolutionTimer.h"

#include "SystemConfig.h"

#include "core/Debug.h"
#include "core/midi/MidiMessage.h"

#include "usbh_core.h"				/// provides usbh_init() and usbh_poll()
#include "usbh_lld_stm32f4.h"		/// provides low level usb host driver for stm32f4 platform
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

static UsbH *g_usbh;

static const usbh_dev_driver_t *device_drivers[] = {
	&usbh_hub_driver,
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

struct MidiDriverHandler {

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
        MidiMessage message;

        switch (code) {
        case 0x0: // (1, 2 or 3 bytes) Miscellaneous function codes. Reserved for future extensions.
        case 0x1: // (1, 2 or 3 bytes) Cable events. Reserved for future expansion.
        case 0x4: // (3 bytes) SysEx starts or continues
        case 0x6: // (2 bytes) SysEx ends with following two bytes.
        case 0x7: // (3 bytes) SysEx ends with following three bytes.
            // ignore for now
            return;
        case 0x5: // (1 bytes) Single-byte System Common Message or SysEx ends with following single byte.
            message = MidiMessage(data[1]);
            g_usbh->midiEnqueueMessage(device_id, message);
            break;
        case 0x2: // (2 bytes) Two-byte System Common messages like MTC, SongSelect, etc.
        case 0xC: // (2 bytes) Program Change
        case 0xD: // (2 bytes) Channel Pressure
            message = MidiMessage(data[1], data[2]);
            g_usbh->midiEnqueueMessage(device_id, message);
            break;
        case 0x3: // (3 bytes) Three-byte System Common messages like SPP, etc.
        case 0x8: // (3 bytes) Note-off
        case 0x9: // (3 bytes) Note-on
        case 0xA: // (3 bytes) Poly-KeyPress
        case 0xB: // (3 bytes) Control Change
        case 0xE: // (3 bytes) PitchBend Change
            message = MidiMessage(data[1], data[2], data[3]);
            g_usbh->midiEnqueueMessage(device_id, message);
            break;
        case 0xF: // (1 bytes) Single Byte
            g_usbh->midiEnqueueData(device_id, data[1]);
            return;
        }
    }

    static void write(uint8_t device, MidiMessage &message) {
        uint8_t data[4];

        data[0] = message.status() >> 4;
        data[1] = message.status();
        data[2] = message.data0();
        data[3] = message.data1();

        usbh_midi_write(device, data, 4, &writeCallback);
    }

    static void writeCallback(uint8_t bytes_written) {
    }
};

static const midi_config_t midi_config = {
    .read_callback = &MidiDriverHandler::recvHandler,
    .notify_connected = &MidiDriverHandler::connectHandler,
    .notify_disconnected = &MidiDriverHandler::disconnectHandler,
};




UsbH::UsbH(UsbMidi &usbMidi) :
    _usbMidi(usbMidi)
{}

void UsbH::init() {
    g_usbh = this;

	rcc_periph_clock_enable(RCC_GPIOA); // OTG_FS + USB_PWR_FAULT
	rcc_periph_clock_enable(RCC_GPIOC); // USB_PWR_EN

    // USB_PWR_EN
	gpio_mode_setup(USB_PWR_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, USB_PWR_EN_PIN);
    gpio_clear(USB_PWR_EN_PORT, USB_PWR_EN_PIN);

    // USB_PWR_FAULT
	gpio_mode_setup(USB_PWR_FAULT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, USB_PWR_FAULT_PIN);

	// periphery
	rcc_periph_clock_enable(RCC_OTGFS); // OTG_FS

	// OTG_FS
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO11 | GPIO12);

	hub_driver_init();
	midi_driver_init(&midi_config);
	usbh_init(lld_drivers, device_drivers);
}

void UsbH::process() {
    uint32_t time_us = HighResolutionTimer::us();

    usbh_poll(time_us);

    // Start sending MIDI messages
    uint8_t device;
    MidiMessage message;
    if (midiDequeueMessage(&device, &message)) {
        if (midiDeviceConnected(device)) {
            MidiDriverHandler::write(device, message);
        }
    }
}

void UsbH::powerOn() {
    gpio_set(USB_PWR_EN_PORT, USB_PWR_EN_PIN);
}

void UsbH::powerOff() {
    gpio_clear(USB_PWR_EN_PORT, USB_PWR_EN_PIN);
}

bool UsbH::powerFault() {
    return !gpio_get(USB_PWR_FAULT_PORT, USB_PWR_FAULT_PIN);
}
