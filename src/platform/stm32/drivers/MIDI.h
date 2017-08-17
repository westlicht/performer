#pragma once

#include "core/midi/MIDIMessage.h"
#include "core/midi/MIDIParser.h"
#include "core/utils/RingBuffer.h"

#include <functional>

#include <cstdint>

class MIDI {
public:
    void init();

    void send(const MIDIMessage &message);
    bool recv(MIDIMessage *message);

    void setRecvFilter(std::function<bool(uint8_t)> filter);

    void handleIrq();
private:
    void send(uint8_t data);

    RingBuffer<uint8_t, 128> _txBuffer;
    RingBuffer<uint8_t, 128> _rxBuffer;
    volatile uint32_t _txActive = 0;

    std::function<bool(uint8_t)> _filter;
    MIDIParser _midiParser;
};
