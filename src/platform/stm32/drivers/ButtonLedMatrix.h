#pragma once

#include "core/utils/RingBuffer.h"

#include <functional>

#include <cstdint>

class ButtonLedMatrix {
public:
    enum Action {
        KeyDown,
        KeyUp,
        // KeyPress,
    };
    struct Event {
        Event() = default;
        Event(Action action, int key) : _action(action), _key(key) {}

        Action action() const { return Action(_action); }
        int key() const { return _key; }

    private:
        uint8_t _action;
        uint8_t _key;
    };

    static const int Rows = 8;
    static const int ButtonCols = 5;
    static const int LedCols = 4;

    ButtonLedMatrix();

    void init();

    void setLed(int index, uint8_t red, uint8_t green) {
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

    ButtonState _buttonState[Rows * ButtonCols];
    LedState _ledState[Rows * LedCols];

    RingBuffer<Event, 32> _events;

    uint8_t _row;
};
