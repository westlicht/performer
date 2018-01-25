#pragma once

#include <cstddef>

template<typename T, size_t Size>
class RingBuffer {
public:
    inline size_t size() const { return _size; }

    inline bool empty() const { return _read == _write; }

    inline bool full() const { return writable() == 0; }

    inline size_t entries() const { return (_write - _read) % _size; }

    inline size_t writable() const {
        return (_read - _write - 1) % _size;
    }

    inline size_t readable() const {
        return (_write - _read) % _size;
    }

    inline void write(T value) {
        size_t write = _write;
        _buffer[write] = value;
        _write = (write + 1) % _size;
    }

    inline void write(const T *data, size_t length) {
        while (length--) {
            write(*data++);
        }
    }

    inline T read() {
        size_t read = _read;
        T value = _buffer[read];
        _read = (read + 1) % _size;
        return value;
    }

    inline void read(T *data, size_t length) {
        while (length--) {
            read(*data++);
        }
    }

private:
    T _buffer[Size];
    const size_t _size = Size;
    volatile size_t _read = 0;
    volatile size_t _write = 0;
};
