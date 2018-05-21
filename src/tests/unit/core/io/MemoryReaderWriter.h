#pragma once

#include <cstring>
#include <cstdint>
#include <cstdio>

class MemoryReader {
public:
    MemoryReader(const void *buf, size_t len) :
        _buf(reinterpret_cast<const uint8_t *>(buf)),
        _len(len),
        _pos(_buf)
    {}

    void read(void *data, size_t len) {
        if (_pos + len < _buf + _len) {
            std::memcpy(data, _pos, len);
            _pos += len;
        }
    }

    size_t bytesRead() const {
        return _pos - _buf;
    }

private:
    const uint8_t *_buf;
    size_t _len;
    const uint8_t *_pos;
};

class MemoryWriter {
public:
    MemoryWriter(void *buf, size_t len) :
        _buf(reinterpret_cast<uint8_t *>(buf)),
        _len(len),
        _pos(_buf)
    {}

    void write(const void *data, size_t len) {
        if (_pos + len < _buf + _len) {
            std::memcpy(_pos, data, len);
            _pos += len;
        }
    }

    size_t bytesWritten() const {
        return _pos - _buf;
    }

private:
    uint8_t *_buf;
    size_t _len;
    uint8_t *_pos;
};
