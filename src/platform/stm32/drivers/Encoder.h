#pragma once

#include "SystemConfig.h"

#include "core/utils/RingBuffer.h"
#include "core/utils/Debouncer.h"

#include <cstdint>

class Encoder {
public:
    enum Event {
        Left,   // encoder turned left
        Right,  // encoder turned right
        Down,   // encoder pressed
        Up,     // encoder released
    };

    void init();

    void process();

    inline bool nextEvent(Event &event) {
        if (_events.readable() < 1) {
            return false;
        }
        event = Event(_events.read());
        return true;
    }

private:
    RingBuffer<uint8_t, 16> _events;

    Debouncer<3> _encoderSwitchDebouncer;
    bool _encoderSwitch = false;
    Debouncer<3> _encoderDebouncer[2];
    bool _encoderState[2] = { false, false };
};
