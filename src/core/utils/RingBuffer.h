#pragma once

#include <cstddef>
#include <cstdint>

template<typename T, int32_t Size>
class RingBuffer {
public:
    inline size_t size() const { return Size; }

    inline bool empty() const { return _read == _write; }

    inline bool full() const { return writable() == 0; }

    inline size_t entries() const {
        int32_t size = (_write - _read) % Size;
        if(size < 0) size += Size;
        return static_cast<size_t>(size);
    }

    inline size_t writable() const {
        return Size - readable();
    }

    inline size_t readable() const {
        return entries();
    }

    inline void write(T value) {
        size_t write = _write;
        _buffer[write] = value;
        _write = (write + 1) % Size;
    }

    inline void write(const T *data, size_t length) {
        while (length--) {
            write(*data++);
        }
    }

    inline T read() {
        size_t read = _read;
        T value = _buffer[read];
        _read = (read + 1) % Size;
        return value;
    }

    inline void read(T *data, size_t length) {
        while (length--) {
            read(*data++);
        }
    }

private:
    T _buffer[Size];
    volatile int32_t _read = 0;
    volatile int32_t _write = 0;
};
