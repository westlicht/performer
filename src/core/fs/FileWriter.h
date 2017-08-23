#pragma once

#include "File.h"

#include <cstring>
#include <cstdint>

namespace fs {

/**
 * File writer.
 * Buffers writes to increase throughput and keeps track of potential errors, which are returned when calling finish().
 */
class FileWriter {
public:
    FileWriter(const char *path) {
        _error = _file.open(path, File::Write);
    }

    ~FileWriter() {
        finish();
    }

    Error error() const { return _error; }

    Error finish() {
        if (_error == OK) {
            _error = _file.write(_buffer, _pos);
        }
        if (_error == OK) {
            _error = _file.close();
        } else {
            _file.close();
        }
        return _error;
    }

    Error write(const void *data, size_t len) {
        const uint8_t *src = static_cast<const uint8_t *>(data);
        while (_error == OK && len > 0) {
            size_t chunk = std::min(len, BufferSize - _pos);
            memcpy(&_buffer[_pos], src, chunk);
            _pos += chunk;
            src += chunk;
            len -= chunk;
            if (_pos == BufferSize) {
                _pos = 0;
                _error = _file.write(_buffer, BufferSize);
            }
        }
        return _error;
    }

private:
    static constexpr size_t BufferSize = 512;

    File _file;
    Error _error;
    uint8_t _buffer[BufferSize];
    size_t _pos = 0;
};

} // namespace fs
