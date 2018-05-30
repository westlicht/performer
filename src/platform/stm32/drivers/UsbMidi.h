#pragma once

#include "core/utils/RingBuffer.h"
#include "core/midi/MidiMessage.h"

#include <functional>

#include <cstdint>

class UsbMidi {
public:
    void init() {}

    void send(const MidiMessage &message) {
        _txQueue.write(message);
    }

    bool recv(MidiMessage *message) {
        if (_rxQueue.empty()) {
            return false;
        }
        *message = _rxQueue.read();
        return true;
    }

    void setRecvFilter(std::function<bool(uint8_t)> filter) {
        _filter = filter;
    }

    void enqueueMessage(MidiMessage &message) {
        _rxQueue.write(message);
    }

    void enqueueData(uint8_t data) {
        if (_filter && !_filter(data)) {
            // _filter(data);
        }
    }

    bool dequeueMessage(MidiMessage *message) {
        if (_txQueue.empty()) {
            return false;
        }
        *message = _txQueue.read();
        return true;
    }

private:
    std::function<bool(uint8_t)> _filter;

    RingBuffer<MidiMessage, 128> _txQueue;
    RingBuffer<MidiMessage, 16> _rxQueue;
};
