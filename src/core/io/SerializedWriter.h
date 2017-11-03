#pragma once

#include <cstdint>

template<typename Writer>
class SerializedWriter {
public:
    SerializedWriter(Writer &writer) :
        _writer(writer)
    {}

    template<typename T>
    void write(const T &value) {
        write(&value, sizeof(value));
    }

    void write(const void *data, size_t len) {
        _writer.write(data, len);
    }

private:
    Writer &_writer;
};
