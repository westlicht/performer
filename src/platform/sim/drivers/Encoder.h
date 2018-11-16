#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"

#include <deque>

class Encoder : private sim::TargetInputHandler {
public:
    enum Event {
        Left,   // encoder turned left
        Right,  // encoder turned right
        Down,   // encoder pressed
        Up,     // encoder released
    };

    Encoder() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    void init() {}

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
    void writeEncoder(sim::EncoderEvent event) override {
        switch (event) {
        case sim::EncoderEvent::Left:   _events.emplace_back(Left);     break;
        case sim::EncoderEvent::Right:  _events.emplace_back(Right);    break;
        case sim::EncoderEvent::Up:     _events.emplace_back(Up);       break;
        case sim::EncoderEvent::Down:   _events.emplace_back(Down);     break;
        }
    }

    sim::Simulator &_simulator;
    std::deque<Event> _events;
};
