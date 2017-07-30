#pragma once

#include "Sequence.h"

#include <array>


template<size_t Size>
struct StaticMemory {
    uint8_t data[Size];

    template<typename T>
    const T &as() const { 
        static_assert(sizeof(T) <= Size, "type is too large");
        return *reinterpret_cast<const T *>(data);
    }
    template<typename T>
    T &as() {
        static_assert(sizeof(T) <= Size, "type is too large");
        return *reinterpret_cast<T *>(data);
    }
};

class Track {
public:
    static const int Num = 8;
    typedef std::array<Track, Num> Array;

    Track() {
        // _data.as<Sequence>() = Sequence();
        reset();
    }

    const Sequence &sequence() const { return _sequence; }
          Sequence &sequence()       { return _sequence; }

    // const Sequence &sequence() const { return _data.as<Sequence>(); }
    //       Sequence &sequence()       { return _data.as<Sequence>(); }

    bool gate() const { return _gate; }

    uint32_t currentStep() const { return _currentStep; }

    void reset() {
        _currentStep = -1;
        _gate = false;
    }

    void tick(uint32_t tick) {
        if (tick % (192 / 4) == 0) {
            _currentStep = (_currentStep + 1) % 16;
            _gate = _sequence.step(_currentStep).active;
        }
        if (tick % (192 / 4) >= (192 / 8)) {
            _gate = false;
        }
    }

private:
    Sequence _sequence;

    bool _gate = false;
    uint32_t _currentStep = -1;

    // StaticMemory<sizeof(Sequence)> _data;
};
