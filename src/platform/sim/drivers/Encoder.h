#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"
#include "sim/widgets/Encoder.h"

#include <deque>

class Encoder {
public:
    enum Event {
        Left,   // encoder turned left
        Right,  // encoder turned right
        Down,   // encoder pressed
        Up,     // encoder released
    };

    Encoder();

    void init() {}

    void process() {}

    bool nextEvent(Event &event);

private:
    sim::Simulator &_simulator;
    sim::Encoder::Ptr _encoder;

    std::deque<Event> _events;
};
