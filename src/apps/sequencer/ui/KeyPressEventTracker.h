#pragma once

#include "Event.h"

#include "os/os.h"

class KeyPressEventTracker {
public:
    KeyPressEvent process(const Key &key) {
        uint32_t currentTicks = os::ticks();
        uint32_t deltaTicks = currentTicks - _lastTicks;

        if (key.code() != _lastCode || deltaTicks > os::time::ms(300)) {
            _count = 1;
        } else {
            ++_count;
        }

        _lastCode = key.code();
        _lastTicks = currentTicks;

        return KeyPressEvent(Event::Type::KeyPress, key, _count);
    }

private:
    uint8_t _lastCode = Key::None;
    uint32_t _lastTicks = 0;
    uint8_t _count = 1;
};
