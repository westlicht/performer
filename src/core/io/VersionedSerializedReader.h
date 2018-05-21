#pragma once

#include <cstdlib>
#include <cstdint>

template<typename Reader>
class VersionedSerializedReader {
public:
    VersionedSerializedReader(Reader &reader, uint32_t readerVersion) :
        _reader(reader),
        _readerVersion(readerVersion)
    {
        _reader.read(&_dataVersion, sizeof(_dataVersion));
    }

    uint32_t readerVersion() const { return _readerVersion; }
    uint32_t dataVersion() const { return _dataVersion; }

    template<typename T>
    void read(T &value, uint32_t addedInVersion = 0) {
        read(&value, sizeof(value), addedInVersion);
    }

    void read(void *data, size_t len, uint32_t addedInVersion) {
        if (_dataVersion >= addedInVersion) {
            _reader.read(data, len);
        }
    }

    template<typename T>
    void skip(uint32_t addedInVersion, uint32_t removedInVersion) {
        skip(sizeof(T), addedInVersion, removedInVersion);
    }

    void skip(size_t len, uint32_t addedInVersion, uint32_t removedInVersion) {
        if (_dataVersion >= addedInVersion && _dataVersion < removedInVersion) {
            uint8_t dummy[len];
            _reader.read(dummy, len);
        }
    }

private:
    Reader &_reader;
    uint32_t _readerVersion;
    uint32_t _dataVersion;
};
