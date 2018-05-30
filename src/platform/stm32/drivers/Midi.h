#pragma once

#include "core/midi/MidiMessage.h"
#include "core/midi/MidiParser.h"
#include "core/utils/RingBuffer.h"

#include <functional>

#include <cstdint>

class Midi {
public:
    void init();

    bool send(const MidiMessage &message);
    bool recv(MidiMessage *message);

    void setRecvFilter(std::function<bool(uint8_t)> filter);

    void handleIrq();
private:
    void send(uint8_t data);

    RingBuffer<uint8_t, 64> _txBuffer;
    RingBuffer<uint8_t, 64> _rxBuffer;
    volatile uint32_t _txActive = 0;

    std::function<bool(uint8_t)> _filter;
    MidiParser _midiParser;
};
