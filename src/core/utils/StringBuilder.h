#pragma once

#include "libs/stb/stb_sprintf.h"

#include <cstdarg>
#include <cstdlib>

class StringBuilder {
public:
    StringBuilder(char *buf, size_t len) :
        _buf(buf),
        _len(len)
    {
        reset();
    }

    void reset() {
        _pos = _buf;
        _buf[0] = 0;
    }

    StringBuilder &printf(const char *fmt, ...) {
        if (fmt) {
            va_list va;
            va_start(va, fmt);
            _pos += stbsp_vsnprintf(_pos, _len - (_pos - _buf), fmt, va);
            va_end(va);
        }
        return *this;
    }

    template<typename ...Args>
    StringBuilder &operator()(char const *fmt, Args... args) {
        return printf(fmt, args...);
    }

    operator const char *() const { return _buf; }

private:
    char *_buf;
    size_t _len;
    char *_pos;
};

/**
 * Class to generate strings with fixed buffer on stack.
 * example: FixedStringBuilder<32>("item %d", 1)(" ")("item %d", 2)
 */
template<size_t Length>
class FixedStringBuilder : public StringBuilder {
public:
    FixedStringBuilder() :
        StringBuilder(_buf, Length)
    {}

    template<typename ...Args>
    FixedStringBuilder(const char *fmt, Args... args) :
        StringBuilder(_buf, Length)
    {
        printf(fmt, args...);
    }

private:
    char _buf[Length];
};
