#pragma once

#include "os/os.h"

template<typename T>
class Atomic {
public:
    Atomic() = default;
    Atomic(const T &value) : _value(value) {}

    inline T set(const T &value) {
        os::InterruptLock lock;
        T old = _value;
        _value = value;
        return old;
    }
private:
    T _value = T(0);
};
