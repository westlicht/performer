#pragma once

#include <cstdint>
#include <functional>

class SerializedReader {
public:
    typedef std::function<void(void *, size_t)> Reader;

    SerializedReader(Reader reader) :
        _reader(reader)
    {}

    template<typename T>
    void read(T &value) {
        read(&value, sizeof(value));
    }

    template<typename ReadT, typename T>
    void readAs(T &value) {
        ReadT tmp;
        read(tmp);
        value = tmp;
    }

    void read(void *data, size_t len) {
        _reader(data, len);
    }

private:
    Reader _reader;
};
