#include "Encoder.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define ENC_PORT GPIOC
#define ENC_SWITCH GPIO15
#define ENC_A GPIO13
#define ENC_B GPIO14
#define ENC_GPIO (ENC_SWITCH | ENC_A | ENC_B)

Encoder::Encoder(bool reverse) :
    _reverse(reverse)
{}

void Encoder::init() {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(ENC_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, ENC_GPIO);
}

void Encoder::process() {
    auto updateEncoder = [this] (int pin, bool state) {
        if (state != _encoderState[pin]) {
            _encoderState[pin] = state;
            if (!_encoderState[0] && !_encoderState[1]) {
                if (_reverse) {
                    _events.write(pin ? Event::Right : Event::Left);
                } else {
                    _events.write(pin ? Event::Left : Event::Right);
                }
            }
        }
    };

    bool state = _encoderSwitchDebouncer.debounce(!gpio_get(ENC_PORT, ENC_SWITCH));
    if (state != _encoderSwitch) {
        _encoderSwitch = state;
        _events.write(state ? Event::Down : Event::Up);
    }
    updateEncoder(0, _encoderDebouncer[0].debounce(!gpio_get(ENC_PORT, ENC_A)));
    updateEncoder(1, _encoderDebouncer[1].debounce(!gpio_get(ENC_PORT, ENC_B)));
}
