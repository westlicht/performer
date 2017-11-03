#pragma once

#include <cstdint>

template<typename Reader>
class SerializedReader {
public:
    SerializedReader(Reader &reader) :
        _reader(reader)
    {}

    template<typename T>
    void read(T &value) {
        read(&value, sizeof(value));
    }

    void read(void *data, size_t len) {
        _reader.read(data, len);
    }

private:
    Reader &_reader;
};
