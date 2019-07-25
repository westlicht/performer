#include "Encoder.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define ENC_PORT GPIOC
#define ENC_SWITCH GPIO15
#define ENC_A GPIO13
#define ENC_B GPIO14
#define ENC_GPIO (ENC_SWITCH | ENC_A | ENC_B)

enum EncoderState {
    Start   = 0,
    CW1     = 1,
    CW2     = 2,
    CW3     = 3,
    CCW1    = 4,
    CCW2    = 5,
    CCW3    = 6,
    CW      = 0x10,
    CCW     = 0x20
};

static const uint8_t encoderStateTable[7][4] = {
    {   Start,  CW1,    CCW1,   Start       },  // Start
    {   CW2,    CW1,    Start,  Start       },  // CW1
    {   CW2,    CW1,    CW3,    Start       },  // CW2
    {   CW2,    Start,  CW3,    Start | CW  },  // CW3
    {   CCW2,   Start,  CCW1,   Start       },  // CCW1
    {   CCW2,   CCW3,   CCW1,   Start       },  // CCW2
    {   CCW2,   CCW3,   Start,  Start | CCW },  // CCW3
};

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

    uint8_t encoderBits =
        ((gpio_get(ENC_PORT, ENC_A) ? 1 : 0)) |
        ((gpio_get(ENC_PORT, ENC_B) ? 1 : 0) << 1);

    _encoderState = encoderStateTable[_encoderState][encoderBits];

    if (_encoderState & CW) {
        _events.write(_reverse ? Event::Right : Event::Left);
    } else if (_encoderState & CCW) {
        _events.write(_reverse ? Event::Left : Event::Right);
    }

    _encoderState &= 0xf;
}
