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
    // handle switch
    bool switchState = _switchDebouncer.debounce(!gpio_get(ENC_PORT, ENC_SWITCH));
    if (switchState != _switchState) {
        _switchState = switchState;
        _events.write(switchState ? Event::Down : Event::Up);
    }

    // handle encoder
    bool encoderState[2] = {
        _encoderDebouncer[0].debounce(!gpio_get(ENC_PORT, ENC_A)),
        _encoderDebouncer[1].debounce(!gpio_get(ENC_PORT, ENC_B))
    };
    if (!_encoderState[0] && !_encoderState[1]) {
        if (encoderState[0] != _encoderState[0]) {
            _events.write(_reverse ? Event::Left : Event::Right);
        }
        if (encoderState[1] != _encoderState[1]) {
            _events.write(_reverse ? Event::Right : Event::Left);
        }
    }
    _encoderState[0] = encoderState[0];
    _encoderState[1] = encoderState[1];
}
