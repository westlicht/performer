#pragma once

#include <algorithm>

#include <cstdint>

template<typename T>
class FrameBuffer {
public:
    FrameBuffer(int width, int height, T *buffer) :
        _width(width),
        _height(height),
        _size(width * height),
        _data(buffer)
    {}

    int width() const { return _width; }
    int height() const { return _height; }

    const T *data() const { return _data; }
          T *data()       { return _data; }

    const T *begin() const { return _data; }
          T *begin()       { return _data; }

    const T *end() const { return &_data[_size]; }
          T *end()       { return &_data[_size]; }

    void fill(const T value) {
        std::fill(begin(), end(), value);
    }

    const T &operator()(int x, int y) const {
        return _data[y * _width + x];
    }

    T &operator()(int x, int y) {
        return _data[y * _width + x];
    }

private:
    int _width;
    int _height;
    int _size;
    T *_data;
};

typedef FrameBuffer<uint8_t> FrameBuffer8bit;
