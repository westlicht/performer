#pragma once

#include "MidiMessage.h"

#include <cstdint>

class MidiParser {
public:
    MidiParser() {
    }

    bool feed(uint8_t data);

    const MidiMessage &message() const {
        return _message;
    }

private:
    uint8_t _status = 0;
    uint8_t _data[2] = { 0, 0 };
    uint8_t _dataIndex = 0;
    uint8_t _dataLength = 0;
    bool _recvSystemExclusive = false;

    MidiMessage _message;
};
