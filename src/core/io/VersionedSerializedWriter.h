#pragma once

#include <cstdlib>
#include <cstdint>

template<typename Writer>
class VersionedSerializedWriter {
public:
    VersionedSerializedWriter(Writer &writer, uint32_t writerVersion) :
        _writer(writer),
        _writerVersion(writerVersion)
    {
        _writer.write(&_writerVersion, sizeof(_writerVersion));
    }

    uint32_t writerVersion() const { return _writerVersion; }

    template<typename T>
    void write(const T &value) {
        write(&value, sizeof(value));
    }

    void write(const void *data, size_t len) {
        _writer.write(data, len);
    }

private:
    Writer &_writer;
    uint32_t _writerVersion;
};
