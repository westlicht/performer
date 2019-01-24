#include "Encoder.h"
#include "InterruptLock.h"

#include "core/utils/Debouncer.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define ENC_PORT GPIOC
#define ENC_SWITCH GPIO15
#define ENC_A GPIO13
#define ENC_B GPIO14
#define ENC_GPIO (ENC_SWITCH | ENC_A | ENC_B)

bool Encoder::_down;
bool Encoder::_pressed;
bool Encoder::_released;
int Encoder::_value;

static Debouncer<3> encoderDebouncer[2];
static bool encoderState[2] = { false, false };

void Encoder::init() {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(ENC_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, ENC_GPIO);

    _down = !gpio_get(ENC_PORT, ENC_SWITCH);
}

void Encoder::deinit() {
    rcc_periph_clock_disable(RCC_GPIOC);
    rcc_periph_reset_release(RST_GPIOC);
}

void Encoder::process() {
    bool lastDown = _down;
    _down = !gpio_get(ENC_PORT, ENC_SWITCH);

    if (!lastDown && _down) {
        _pressed = true;
    }
    if (lastDown && !_down) {
        _released = true;
    }

    auto updateEncoder = [&] (int pin, bool state) {
        if (state != encoderState[pin]) {
            encoderState[pin] = state;
            if (!encoderState[0] && !encoderState[1]) {
                _value += pin ? 1 : -1;
            }
        }
    };

    updateEncoder(0, encoderDebouncer[0].debounce(!gpio_get(ENC_PORT, ENC_A)));
    updateEncoder(1, encoderDebouncer[1].debounce(!gpio_get(ENC_PORT, ENC_B)));
}

void Encoder::reset() {
    InterruptLock lock;
    _pressed = false;
    _released = false;
    _value = false;
}

bool Encoder::pressed() {
    InterruptLock lock;
    bool result = _pressed;
    _pressed = false;
    return result;
}

bool Encoder::released() {
    InterruptLock lock;
    bool result = _released;
    _released = false;
    return result;
}

int Encoder::value() {
    InterruptLock lock;
    int result = _value;
    _value = 0;
    return result;
}
