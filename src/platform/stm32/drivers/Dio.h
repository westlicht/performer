#pragma once

#include <libopencm3/stm32/gpio.h>

#include <functional>

class Dio {
public:
    template<uint32_t Port, uint32_t Pin>
    struct Input {
        typedef std::function<void(bool)> Handler;

        void init() {
            gpio_mode_setup(Port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, Pin);
            update();
        }

        bool get() const {
            return _state;
        }

        void setHandler(Handler handler) { _handler = handler; }

        void interrupt() {
            update();
            if (_handler) {
                _handler(_state);
            }
        }

    private:
        void update() {
            _state = !gpio_get(Port, Pin);
        }

        bool _state = false;
        Handler _handler;
    };

    template<uint32_t Port, uint32_t Pin>
    struct Output {
        void init() {
            gpio_mode_setup(Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, Pin);
        }

        void set(bool value) {
            if (value) {
                gpio_clear(Port, Pin);
            } else {
                gpio_set(Port, Pin);
            }
        }
    };

    Dio();

    void init();

    Input<GPIOB, GPIO11> clockInput;
    Input<GPIOB, GPIO10> resetInput;

    Output<GPIOB, GPIO2> clockOutput;
    Output<GPIOB, GPIO1> resetOutput;
};
