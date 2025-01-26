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
#include <libopencm3/usb/dwc/otg_hs.h>
#include <libopencm3/usb/dwc/otg_fs.h>

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

static constexpr size_t WriteBufferSize = 64;
static uint8_t writeBuffer[2][WriteBufferSize];
static size_t writeBufferSize;
static size_t writeBufferIndex;
static size_t writeBufferPos;

struct MidiDriverHandler {

    static void connectHandler(int device, uint16_t vendorId, uint16_t productId, uint32_t maxPacketSize) {
        DBG("MIDI device connected (id=%d, vendorId=%04x, productId=%04x, maxPacketSize=%ld)", device, vendorId, productId, maxPacketSize);
        g_usbh->midiConnectDevice(device, vendorId, productId);
        writeBufferSize = std::min(WriteBufferSize, size_t(maxPacketSize));
    }

    static void disconnectHandler(int device) {
        DBG("MIDI device disconnected (id=%d)", device);
        g_usbh->midiDisconnectDevice(device);
    }

    static void recvHandler(int device, uint8_t *data) {
        uint8_t cable = data[0] >> 4;
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
            g_usbh->midiEnqueueMessage(device, cable, message);
            break;
        case 0x2: // (2 bytes) Two-byte System Common messages like MTC, SongSelect, etc.
        case 0xC: // (2 bytes) Program Change
        case 0xD: // (2 bytes) Channel Pressure
            message = MidiMessage(data[1], data[2]);
            g_usbh->midiEnqueueMessage(device, cable, message);
            break;
        case 0x3: // (3 bytes) Three-byte System Common messages like SPP, etc.
        case 0x8: // (3 bytes) Note-off
        case 0x9: // (3 bytes) Note-on
        case 0xA: // (3 bytes) Poly-KeyPress
        case 0xB: // (3 bytes) Control Change
        case 0xE: // (3 bytes) PitchBend Change
            message = MidiMessage(data[1], data[2], data[3]);
            g_usbh->midiEnqueueMessage(device, cable, message);
            break;
        case 0xF: // (1 bytes) Single Byte
            g_usbh->midiEnqueueData(device, cable, data[1]);
            return;
        }
    }

    static bool write(uint8_t device, uint8_t cable, const MidiMessage &message) {
        bool flushed = true;

        if (message.isSystemExclusive()) {
            const uint8_t *payloadData = message.payloadData();
            size_t payloadLength = message.payloadLength();
            if (payloadData && payloadLength > 0) {
                size_t messageLength = payloadLength + 2;
                size_t writeSize = ((messageLength + 3) / 4) * 4;
                if (writeBufferPos + writeSize >= writeBufferSize) {
                    flush(device);
                    flushed = true;
                }

                size_t messageIndex = 0;
                while (messageIndex < messageLength) {
                    uint8_t *p = &writeBuffer[writeBufferIndex][writeBufferPos];
                    size_t chunkSize;
                    switch (messageLength - messageIndex) {
                    case 1:
                        p[0] = 0x5;
                        chunkSize = 1;
                        break;
                    case 2:
                        p[0] = 0x6;
                        chunkSize = 2;
                        break;
                    case 3:
                        p[0] = 0x7;
                        chunkSize = 3;
                        break;
                    default:
                        p[0] = 0x4;
                        chunkSize = 3;
                        break;
                    }
                    p[0] |= cable << 4;
                    for (size_t i = 0; i < chunkSize; ++i) {
                        uint8_t byte;
                        if (messageIndex == 0) {
                            byte = 0xf0;
                        } else if (messageIndex == messageLength - 1) {
                            byte = 0xf7;
                        } else {
                            byte = payloadData[messageIndex - 1];
                        }
                        p[1 + i] = byte;
                        ++messageIndex;
                    }
                    for (size_t i = chunkSize; i < 3; ++i) {
                        p[1 + i] = 0;
                    }
                    writeBufferPos += 4;
                }
            }
        } else {
            if (writeBufferPos + 4 >= writeBufferSize) {
                flush(device);
                flushed = true;
            }
            uint8_t *p = &writeBuffer[writeBufferIndex][writeBufferPos];
            p[0] = message.status() >> 4 | (cable << 4);
            p[1] = message.status();
            p[2] = message.data0();
            p[3] = message.data1();
            writeBufferPos += 4;
        }

        return flushed;
    }

    static void flush(uint8_t device) {
        if (writeBufferPos > 0) {
            usbh_midi_write(device, writeBuffer[writeBufferIndex], writeBufferPos, &writeCallback);
            writeBufferIndex = (writeBufferIndex + 1) % 2;
            writeBufferPos = 0;
        }
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
    uint8_t cable;
    MidiMessage message;
    bool flushed = false;
    while (midiDequeueMessage(&device, &cable, &message)) {
        if (midiDeviceConnected(device)) {
            flushed = MidiDriverHandler::write(device, cable, message);
            if (flushed) {
                break;
            }
        }
    }
    if (!flushed) {
        MidiDriverHandler::flush(device);
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
