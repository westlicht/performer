#pragma once

#include "sim/Simulator.h"

#include <functional>
#include <deque>

#include <cstdint>

class MIDI {
public:
    MIDI(sim::Simulator &simulator) :
        _simulator(simulator)
    {
        _simulator.recvMIDI(sim::Simulator::MIDIHardwarePort, [this] (uint8_t data) {
            if (!_filter || !_filter(data)) {
                _recvQueue.emplace_back(data);
            }
        });
    }

    void send(uint8_t data) {
        _simulator.sendMIDI(sim::Simulator::MIDIHardwarePort, data);
    }

    bool recv(uint8_t *data) {
        if (_recvQueue.empty()) {
            return false;
        } else {
            *data = _recvQueue.front();
            _recvQueue.pop_front();
            return true;
        }
    }

    void setRecvFilter(std::function<bool(uint8_t)> filter) {
        _filter = filter;
    }

private:
    sim::Simulator &_simulator;
    std::deque<uint8_t> _recvQueue;
    std::function<bool(uint8_t)> _filter;
};
