#pragma once

#include "core/utils/RingBuffer.h"
#include "core/midi/MIDIMessage.h"

#include <functional>

#include <cstdint>

class USBMIDI {
public:
    void init() {}

    void send(const MIDIMessage &message) {
        _txQueue.write(message);
    }

    bool recv(MIDIMessage *message) {
        if (_rxQueue.empty()) {
            return false;
        }
        *message = _rxQueue.read();
        return true;
    }

    void setRecvFilter(std::function<bool(uint8_t)> filter) {
        _filter = filter;
    }

    void enqueueMessage(MIDIMessage &message) {
        _rxQueue.write(message);
    }

    void enqueueData(uint8_t data) {
        if (_filter && !_filter(data)) {
            // _filter(data);
        }
    }

    bool dequeueMessage(MIDIMessage *message) {
        if (_txQueue.empty()) {
            return false;
        }
        *message = _txQueue.read();
        return true;
    }

private:
    std::function<bool(uint8_t)> _filter;

    RingBuffer<MIDIMessage, 16> _txQueue;
    RingBuffer<MIDIMessage, 16> _rxQueue;
};
