#pragma once

#include <cstdint>

template<uint8_t Count>
class Debouncer {
public:
    bool debounce(bool state) {
        if (state != _state) {
            if (++_counter >= Count) {
                _state = state;
                _counter = 0;
            }
        } else {
            _counter = 0;
        }

        return _state;
    }

private:
    bool _state = false;
    uint8_t _counter = 0;
};
