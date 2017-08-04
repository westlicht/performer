#pragma once

#include "libs/stb/stb_sprintf.h"

#include <cstdarg>

class BaseStringBuilder {
public:
    BaseStringBuilder(char *buf, size_t len) :
        _buf(buf),
        _len(len)
    {
        reset();
    }

    void reset() {
        _pos = _buf;
    }

    BaseStringBuilder &printf(const char *fmt, ...) {
        va_list va;
        va_start(va, fmt);
        _pos += stbsp_vsnprintf(_pos, _len - (_pos - _buf), fmt, va);
        va_end(va);
        return *this;
    }

    template<typename ...Args>
    BaseStringBuilder &operator()(char const *fmt, Args... args) {
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
 * example: StringBuilder<32>("item %d", 1)(" ")("item %d", 2)
 */
template<size_t Length>
class StringBuilder : public BaseStringBuilder {
public:
    StringBuilder() :
        BaseStringBuilder(_buf, Length)
    {}

    template<typename ...Args>
    StringBuilder(const char *fmt, Args... args) :
        BaseStringBuilder(_buf, Length)
    {
        printf(fmt, args...);
    }

private:
    char _buf[Length];
};
