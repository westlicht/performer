#pragma once

#include <algorithm>

template<typename T, size_t History>
class MovingAverage {
public:
    MovingAverage() {
        reset();
    }

    void reset() {
        for (size_t i = 0; i < History; ++i) {
            _history[i] = T(0);
        }
        _sum = T(0);
        _index = 0;
        _count = 0;
    }

    void push(T sample) {
        _sum -= _history[_index];
        _sum += sample;
        _history[_index] = sample;
        _index = (_index + 1) % History;
        _count = std::min(_count + 1, History);
    }

    T operator()() const {
        return _sum / (_count == 0 ? 1 : _count);
    }

private:
    T _history[History];
    T _sum = T(0);
    size_t _index = 0;
    size_t _count = 0;
};
