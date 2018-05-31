#pragma once

#include "File.h"

#include <cstring>
#include <cstddef>
#include <cstdint>

namespace fs {

/**
 * File reader.
 * Buffers reads to increase throughput and keeps track of potential errors, which are returned when calling finish().
 */
class FileReader {
public:
    FileReader(const char *path) {
        _error = _file.open(path, File::Read);
    }

    ~FileReader() {
        finish();
    }

    Error error() const { return _error; }

    Error finish() {
        if (!_finished) {
            if (_error == OK) {
                _error = _file.close();
            } else {
                _file.close();
            }
            _finished = true;
        }
        return _error;
    }

    Error read(void *data, size_t len) {
        uint8_t *dst = static_cast<uint8_t *>(data);
        uint8_t *buffer = reinterpret_cast<uint8_t *>(_buffer);
        while (_error == OK && len > 0) {
            if (_pos == 0 || _pos == BufferSize) {
                _error = _file.read(buffer, BufferSize, &_bufferSize);
                if (_error != OK) {
                    break;
                }
                _pos = 0;
            }
            size_t chunk = std::min(len, _bufferSize - _pos);
            if (chunk == 0) {
                _error = END_OF_FILE;
                break;
            }
            memcpy(dst, &buffer[_pos], chunk);
            _pos += chunk;
            dst += chunk;
            len -= chunk;
        }
        return _error;
    }

private:
    static constexpr size_t BufferSize = 512;

    File _file;
    bool _finished = false;
    Error _error;
    uint32_t _buffer[BufferSize / 4];
    size_t _bufferSize = 0;
    size_t _pos = 0;
};

} // namespace fs
