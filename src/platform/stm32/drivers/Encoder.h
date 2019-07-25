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

    Encoder(bool reverse = false);

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
    bool _reverse;

    RingBuffer<uint8_t, 32> _events;

    Debouncer<3> _switchDebouncer;
    bool _switchState = false;
    uint8_t _encoderState = 0;
};
