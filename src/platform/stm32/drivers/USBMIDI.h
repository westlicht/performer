#pragma once

#include <functional>

#include <cstdint>

class USBMIDI {
public:
    void init() {}

    void send(uint8_t data) {
    }

    bool recv(uint8_t *data) {
        return false;
    }

    void setRecvFilter(std::function<bool(uint8_t)> filter) {
        _filter = filter;
    }

private:
    std::function<bool(uint8_t)> _filter;
};
