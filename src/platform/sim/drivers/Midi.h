#pragma once

#include "core/midi/MidiMessage.h"

#include "sim/Simulator.h"

#include <functional>
#include <deque>

#include <cstdint>

class Midi : private sim::TargetInputHandler {
public:
    typedef std::function<bool(uint8_t)> RecvFilter;

    Midi() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    void init() {}

    bool send(const MidiMessage &message) {
        _simulator.writeMidiOutput(sim::MidiEvent::makeMessage(0, message));
        return true;
    }

    bool recv(MidiMessage *message) {
        if (!_recvQueue.empty()) {
            *message = _recvQueue.front();
            _recvQueue.pop_front();
            return true;
        }
        return false;
    }

    void setRecvFilter(RecvFilter filter) {
        _recvFilter = filter;
    }

    uint32_t rxOverflow() const { return 0; }

private:
    void writeMidiInput(sim::MidiEvent event) {
        if (event.port == 0 && event.kind == sim::MidiEvent::Message) {
            if (event.message.length() != 1 || !_recvFilter || !_recvFilter(event.message.status())) {
                _recvQueue.emplace_back(event.message);
            }
        }
    }

    sim::Simulator &_simulator;
    std::deque<MidiMessage> _recvQueue;
    RecvFilter _recvFilter;
};
