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
    }

    const Sequence &sequence() const { return _sequence; }
          Sequence &sequence()       { return _sequence; }

    // const Sequence &sequence() const { return _data.as<Sequence>(); }
    //       Sequence &sequence()       { return _data.as<Sequence>(); }

private:
    Sequence _sequence;

    // StaticMemory<sizeof(Sequence)> _data;
};
