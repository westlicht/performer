#pragma once

#include "MIDIMessage.h"

#include <cstdint>

class MIDIParser {
public:
    MIDIParser() {
    }

    bool feed(uint8_t data);

    const MIDIMessage &message() const {
        return _message;
    }

private:
    uint8_t _status = 0;
    uint8_t _data[2] = { 0, 0 };
    uint8_t _dataIndex = 0;
    uint8_t _dataLength = 0;
    bool _recvSystemExclusive = false;

    MIDIMessage _message;
};
