#pragma once

#include "core/hash/FnvHash.h"

#include <cstdlib>
#include <cstdint>
#include <functional>

class VersionedSerializedWriter {
public:
    typedef std::function<void(const void *, size_t)> Writer;

    VersionedSerializedWriter(Writer writer, uint32_t writerVersion) :
        _writer(writer),
        _writerVersion(writerVersion)
    {
        _writer(&_writerVersion, sizeof(_writerVersion));
    }

    uint32_t writerVersion() const { return _writerVersion; }

    template<typename T>
    void write(const T &value) {
        write(&value, sizeof(value));
    }

    template<typename Enum, typename SerializeFunc>
    void writeEnum(Enum e, SerializeFunc serialize) {
        auto value = serialize(e);
        write(value);
    }

    void write(const void *data, size_t len) {
        _hash(data, len);
        _writer(data, len);
    }

    void writeHash() {
        uint32_t hash = _hash.result();
        _writer(&hash, sizeof(hash));
    }

private:
    Writer _writer;
    uint32_t _writerVersion;
    FnvHash _hash;
};
