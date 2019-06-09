#pragma once

#include "SystemConfig.h"

#include "core/utils/RingBuffer.h"
#include "core/utils/Debouncer.h"

#include "drivers/ShiftRegister.h"

#include <array>
#include <utility>

#include <cstdint>
#include <cstdlib>

class ButtonLedMatrix {
public:
    struct Event {
        enum Action {
            KeyDown,
            KeyUp,
        };

        Event() = default;
        Event(Action action, int value) : _action(action), _value(value) {}

        Action action() const { return Action(_action); }
        int value() const { return _value; }

    private:
        uint8_t _action;
        int8_t _value;
    };

    static constexpr int Rows = CONFIG_BLM_ROWS;
    static constexpr int ColsButton = CONFIG_BLM_COLS_BUTTON;
    static constexpr int ColsLed = CONFIG_BLM_COLS_LED;

    ButtonLedMatrix(ShiftRegister &shiftRegister, bool invertLeds = false);

    void init();

    void setLed(int index, uint8_t red, uint8_t green) {
        if (_invertLeds) {
            std::swap(red, green);
        }
        _ledState[index].red.intensity = red >> 4;
        _ledState[index].green.intensity = green >> 4;
        if (red == 0) {
            _ledState[index].red.counter = 0;
        }
        if (green == 0) {
            _ledState[index].green.counter = 0;
        }
    }

    inline void setLed(int row, int col, uint8_t red, uint8_t green) {
        setLed(col * Rows + row, red, green);
    }

    void setLeds(const std::array<std::pair<uint8_t, uint8_t>, Rows * ColsLed> &leds) {
        for (size_t i = 0; i < leds.size(); ++i) {
            setLed(i, leds[i].first, leds[i].second);
        }
    }

    inline bool buttonState(int index) const {
        return _buttonState[index].state;
    }

    inline bool buttonState(int row, int col) const {
        return buttonState(col * Rows + row);
    }

    void process();

    inline bool nextEvent(Event &event) {
        if (_events.readable() < 1) {
            return false;
        }
        event = _events.read();
        return true;
    }

private:
    struct Led {
        uint8_t intensity : 4;
        uint8_t counter : 4;
        inline bool update() {
            // return true;
            uint8_t sum = counter + intensity;
            bool active = sum >= 0x0f;
            counter = sum & 0x0f;
            return active;
        };
    };

    struct LedState {
        Led red;
        Led green;
    } __attribute__((packed));

    struct ButtonState {
        uint8_t state;
        // uint8_t counter : 7;
    };

    ShiftRegister &_shiftRegister;
    bool _invertLeds;

    ButtonState _buttonState[Rows * ColsButton];
    LedState _ledState[Rows * ColsLed];

    RingBuffer<Event, 16> _events;

    uint8_t _row = 0;
};
