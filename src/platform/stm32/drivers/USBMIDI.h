#pragma once

#include <functional>

#include <cstdint>

class USBMIDI {
public:
    void init() {}

    void send(const MIDIMessage &message) {}
    bool recv(MIDIMessage *message) { return false; }

    void setRecvFilter(std::function<bool(uint8_t)> filter) {
        _filter = filter;
    }

private:
    std::function<bool(uint8_t)> _filter;
};
