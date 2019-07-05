#pragma once

#include "core/hash/FnvHash.h"

#include <cstdlib>
#include <cstdint>
#include <functional>

class VersionedSerializedReader {
public:
    typedef std::function<void(void *, size_t)> Reader;

    VersionedSerializedReader(Reader reader, uint32_t readerVersion) :
        _reader(reader),
        _readerVersion(readerVersion)
    {
        _reader(&_dataVersion, sizeof(_dataVersion));
    }

    uint32_t readerVersion() const { return _readerVersion; }
    uint32_t dataVersion() const { return _dataVersion; }

    template<typename T>
    void read(T &value, uint32_t addedInVersion = 0) {
        read(&value, sizeof(value), addedInVersion);
    }

    // This is potentially slow as we call serialize for every enum value until a match is found.
    // However, the nice thing about using a switch state in the serialize function is that the
    // compiler can warn us when new enum values are added without updating the serialize function.
    template<typename Enum, typename SerializeFunc>
    void readEnum(Enum &e, SerializeFunc serialize, uint32_t addedInVersion = 0) {
        if (_dataVersion >= addedInVersion) {
            auto i = serialize(Enum(0));
            read(i);
            for (e = Enum(0); int(e) < int(Enum::Last); e = Enum(int(e) + 1)) {
                if (serialize(e) == i) {
                    return;
                }
            }
            e = Enum(0);
        }
    }

    template<typename ReadT, typename T>
    void readAs(T &value, uint32_t addedInVersion = 0) {
        if (_dataVersion >= addedInVersion) {
            ReadT tmp;
            read(tmp);
            value = tmp;
        }
    }

    void read(void *data, size_t len, uint32_t addedInVersion) {
        if (_dataVersion >= addedInVersion) {
            _reader(data, len);
            _hash(data, len);
        }
    }

    template<typename T>
    void skip(uint32_t addedInVersion, uint32_t removedInVersion) {
        skip(sizeof(T), addedInVersion, removedInVersion);
    }

    void skip(size_t len, uint32_t addedInVersion, uint32_t removedInVersion) {
        if (_dataVersion >= addedInVersion && _dataVersion < removedInVersion) {
            uint8_t dummy[len];
            _reader(dummy, len);
            _hash(dummy, len);
        }
    }

    bool checkHash() {
        uint32_t hash;
        _reader(&hash, sizeof(hash));
        return _hash.result() == hash;
    }

private:
    Reader _reader;
    uint32_t _readerVersion;
    uint32_t _dataVersion;
    FnvHash _hash;
};
