#include "USBH.h"
#include "HighResolutionTimer.h"

#include "Config.h"

#include "core/Debug.h"
#include "core/midi/MIDIMessage.h"

#include "usbh_core.h"				/// provides usbh_init() and usbh_poll()
#include "usbh_lld_stm32f4.h"		/// provides low level usb host driver for stm32f4 platform
#include "usbh_driver_hid.h"		/// provides generic usb device driver for Human Interface Device (HID)
#include "usbh_driver_hub.h"		/// provides usb full speed hub driver (Low speed devices on hub are not supported)
// #include "usbh_driver_gp_xbox.h"	/// provides usb device driver for Gamepad: Microsoft XBOX compatible Controller
#include "usbh_driver_ac_midi.h"	/// provides usb device driver for midi class devices

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/otg_hs.h>
#include <libopencm3/stm32/otg_fs.h>

static USBH *g_usbh;

static const usbh_dev_driver_t *device_drivers[] = {
	&usbh_hub_driver,
	&usbh_hid_driver,
	// &usbh_gp_xbox_driver,
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

static void hid_in_message_handler(uint8_t device_id, const uint8_t *data, uint32_t length)
{
	(void)device_id;
	(void)data;
	if (length < 4) {
		DBG("data too short, type=%d", hid_get_type(device_id));
		return;
	}

	// print only first 4 bytes, since every mouse should have at least these four set.
	// Report descriptors are not read by driver for now, so we do not know what each byte means
	// DBG("HID EVENT %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
	if (hid_get_type(device_id) == HID_TYPE_KEYBOARD) {
		static int x = 0;
		if (x != data[2]) {
			x = data[2];
			hid_set_report(device_id, x);
		}
	}
    if (hid_get_type(device_id) == HID_TYPE_MOUSE) {
        static int x, y;
        x += (int8_t) data[1];
        y += (int8_t) data[2];
        g_usbh->setMousePos(x, y);
        // DBG("x = %d, y = %d", x, y);
    }
}

static const hid_config_t hid_config = {
	.hid_in_message_handler = &hid_in_message_handler
};

static void midi_in_message_handler(int device_id, uint8_t *data) {
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
        break;
    case 0x2: // (2 bytes) Two-byte System Common messages like MTC, SongSelect, etc.
    case 0xC: // (2 bytes) Program Change
    case 0xD: // (2 bytes) Channel Pressure
        message = MIDIMessage(data[1], data[2]);
        break;
    case 0x3: // (3 bytes) Three-byte System Common messages like SPP, etc.
    case 0x8: // (3 bytes) Note-off
    case 0x9: // (3 bytes) Note-on
    case 0xA: // (3 bytes) Poly-KeyPress
    case 0xB: // (3 bytes) Control Change
    case 0xE: // (3 bytes) PitchBend Change
        message = MIDIMessage(data[1], data[2], data[3]);
        break;
    case 0xF: // (1 bytes) Single Byte
        // needs parsing
        DBG("usb midi data (raw)");
        return;
    }

    DBG("usb midi event");
    MIDIMessage::dump(message);
}

static void midi_connected_handler(int device_id) {
    DBG("MIDI device connected (id=%d)", device_id);
}

static void midi_disconnected_handler(int device_id) {
    DBG("MIDI device disconnected (id=%d)", device_id);
}

const midi_config_t midi_config = {
	.read_callback = &midi_in_message_handler,
	.notify_connected = &midi_connected_handler,
	.notify_disconnected = &midi_disconnected_handler,
};


void USBH::init() {
    g_usbh = this;

	rcc_periph_clock_enable(RCC_GPIOA); // OTG_FS + button
	rcc_periph_clock_enable(RCC_GPIOB); // OTG_HS
	rcc_periph_clock_enable(RCC_GPIOG); // USB OTG Power Up
	// rcc_periph_clock_enable(RCC_GPIOC); // USART + OTG_FS charge pump
	// rcc_periph_clock_enable(RCC_GPIOD); // LEDS

	// periphery
	// rcc_periph_clock_enable(RCC_USART6); // USART
	rcc_periph_clock_enable(RCC_OTGFS); // OTG_FS
	rcc_periph_clock_enable(RCC_OTGHS); // OTG_HS
	// rcc_periph_clock_enable(RCC_TIM6); // TIM6

    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
    gpio_set(GPIOG, GPIO6);


	// /* Set GPIO12-15 (in GPIO port D) to 'output push-pull'. */
	// gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
	// 		GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	/* Set	 */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
	gpio_clear(GPIOC, GPIO0);

	// gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
	// 		GPIO13 | GPIO14 | GPIO15);
	// gpio_set_af(GPIOB, GPIO_AF12, GPIO13 | GPIO14 | GPIO15);



	// OTG_FS
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO11 | GPIO12);

	// OTG_HS
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15 | GPIO14);
	gpio_set_af(GPIOB, GPIO_AF12, GPIO14 | GPIO15);

	hid_driver_init(&hid_config);
	hub_driver_init();
	midi_driver_init(&midi_config);


	// gpio_set(GPIOD,  GPIO13);
	usbh_init(lld_drivers, device_drivers);
	// gpio_clear(GPIOD,  GPIO13);

	DBG("USBH init complete");

}

void USBH::process() {
    // set busy led
    // gpio_set(GPIOD,  GPIO14);

    // uint32_t time_curr_us = tim6_get_time_us();
    uint32_t time_us = HighResolutionTimer::us();

    usbh_poll(time_us);

    // clear busy led
    // gpio_clear(GPIOD,  GPIO14);

    // LOG_FLUSH();

    // approx 1ms interval between usbh_poll()
    // delay_ms_busy_loop(1);

}
