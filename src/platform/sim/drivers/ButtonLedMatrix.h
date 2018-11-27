#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"

#include <deque>
#include <array>
#include <bitset>

#include <cstdint>

class ButtonLedMatrix : private sim::TargetInputHandler {
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

    ButtonLedMatrix() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    void init() {}

    void setLed(int index, uint8_t red, uint8_t green) {
        _simulator.writeLed(index, red > 0, green > 0);
    }

    void setLed(int row, int col, uint8_t red, uint8_t green) {
        setLed(col * Rows + row, red, green);
    }

    void setLeds(const std::array<std::pair<uint8_t, uint8_t>, Rows * ColsLed> &leds) {
        for (size_t i = 0; i < leds.size(); ++i) {
            setLed(i, leds[i].first, leds[i].second);
        }
    }

    bool buttonState(int index) const {
        return _buttonState[index];
    }

    inline bool buttonState(int row, int col) const {
        return buttonState(col * Rows + row);
    }

    void process() {}

    bool nextEvent(Event &event) {
        if (_events.empty()) {
            return false;
        }
        event = _events.front();
        _events.pop_front();
        return true;
    }

private:
    void writeButton(int index, bool pressed) override {
        if (index < 0 || index >= 37) {
            return;
        }
        if (_buttonState[index] != pressed) {
            _buttonState[index] = pressed;
            _events.emplace_back(pressed ? Event::KeyDown : Event::KeyUp, index);
        }
    }

    sim::Simulator &_simulator;
    std::bitset<Rows * ColsButton> _buttonState;
    std::deque<Event> _events;
};
