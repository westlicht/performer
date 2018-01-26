#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"
#include "sim/widgets/Button.h"
#include "sim/widgets/Led.h"
#include "sim/widgets/Label.h"

#include <vector>
#include <deque>
#include <array>
#include <utility>

#include <cstdint>

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

    static const int Rows = CONFIG_BLM_ROWS;
    static const int ColsButton = CONFIG_BLM_COLS_BUTTON;
    static const int ColsLed = CONFIG_BLM_COLS_LED;

    ButtonLedMatrix();

    void init() {}

    void setLed(int index, uint8_t red, uint8_t green);

    void setLed(int row, int col, uint8_t red, uint8_t green) {
        setLed(col * Rows + row, red, green);
    }

    void setLeds(const std::array<std::pair<uint8_t, uint8_t>, Rows * ColsLed> &leds) {
        for (size_t i = 0; i < leds.size(); ++i) {
            setLed(i, leds[i].first, leds[i].second);
        }
    }

    bool buttonState(int index) const;

    inline bool buttonState(int row, int col) const {
        return buttonState(col * Rows + row);
    }

    void process() {}

    bool nextEvent(Event &event);

private:
    sim::Simulator &_simulator;
    std::vector<sim::Button::Ptr> _buttons;
    std::vector<sim::Led::Ptr> _leds;
    std::vector<sim::Label::Ptr> _labels;
    std::deque<Event> _events;
};
