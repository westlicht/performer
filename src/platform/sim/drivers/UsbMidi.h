#pragma once

#include "core/midi/MidiMessage.h"
#include "core/midi/MidiParser.h"

#include "sim/Simulator.h"

#include <functional>
#include <deque>
#include <mutex>

#include <cstdint>

class UsbMidi {
public:
    UsbMidi() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.recvMidi(sim::Simulator::MidiUsbHostPort, [this] (uint8_t data) {
            if (!_filter || !_filter(data)) {
                std::lock_guard<std::mutex> lock(_recvMutex);
                _recvQueue.emplace_back(data);
            }
        });
    }

    void init() {}

    void send(const MidiMessage &message) {
        _simulator.sendMidi(sim::Simulator::MidiUsbHostPort, message.raw(), message.length());
    }

    bool recv(MidiMessage *message) {
        std::lock_guard<std::mutex> lock(_recvMutex);
        while (!_recvQueue.empty()) {
            uint8_t data = _recvQueue.front();
            _recvQueue.pop_front();
            if (_midiParser.feed(data)) {
                *message = _midiParser.message();
                return true;
            }
        }
        return false;
    }

    void setRecvFilter(std::function<bool(uint8_t)> filter) {
        _filter = filter;
    }

private:
    sim::Simulator &_simulator;
    std::deque<uint8_t> _recvQueue;
    std::mutex _recvMutex;
    std::function<bool(uint8_t)> _filter;
    MidiParser _midiParser;
};
