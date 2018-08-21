#pragma once

#include <algorithm>

#include <cstring>

template<size_t Lines, size_t LineLength>
class LogBuffer {
public:
    void clear() {
        _index = 0;
        _lines = 0;
    }

    void print(const char *text) {
        strncpy(_buffer[_index], text, LineLength);
        _buffer[_index][LineLength - 1] = '\0';

        _index = (_index + 1) % Lines;
        _lines = std::min(_lines + 1, Lines);
    }

    size_t lines() const {
        return _lines;
    };

    const char *line(int index) const {
        index = (index + _index + Lines - _lines) % Lines;
        return _buffer[index];
    }
private:
    size_t _index = 0;
    size_t _lines = 0;
    char _buffer[Lines][LineLength];
};
