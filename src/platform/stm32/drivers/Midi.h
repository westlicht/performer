#pragma once

#include "core/midi/MidiMessage.h"
#include "core/midi/MidiParser.h"
#include "core/utils/RingBuffer.h"

#include <functional>

#include <cstdint>

class Midi {
public:
    typedef std::function<bool(uint8_t)> RecvFilter;

    void init();

    bool send(const MidiMessage &message);
    bool recv(MidiMessage *message);

    void setRecvFilter(RecvFilter filter);

    uint32_t rxOverflow() const { return _rxOverflow; }

    void handleIrq();
private:
    void send(uint8_t data);

    RingBuffer<uint8_t, 64> _txBuffer;
    RingBuffer<uint8_t, 64> _rxBuffer;
    volatile uint32_t _rxOverflow = 0;
    volatile uint32_t _txActive = 0;

    RecvFilter _recvFilter;
    MidiParser _midiParser;
};
