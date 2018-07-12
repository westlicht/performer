#pragma once

#include "core/midi/MidiMessage.h"
#include "core/midi/MidiParser.h"

#include "sim/Simulator.h"
#include "sim/MidiConfig.h"

#include <functional>
#include <deque>
#include <mutex>
#include <memory>

#include <cstdint>

class Midi {
public:
    Midi() :
        _simulator(sim::Simulator::instance())
    {
        _port = std::make_shared<sim::Midi::Port>(
            sim::midiPortConfig.port,
            [this] (uint8_t data) {
                if (!_filter || !_filter(data)) {
                    std::lock_guard<std::mutex> lock(_recvMutex);
                    _recvQueue.emplace_back(data);
                }
            }
        );

        _simulator.midi().registerPort(_port);
    }

    void init() {}

    bool send(const MidiMessage &message) {
        return _port->send(message.raw(), message.length());
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

    uint32_t rxOverflow() const { return 0; }

private:
    sim::Simulator &_simulator;
    std::shared_ptr<sim::Midi::Port> _port;
    std::deque<uint8_t> _recvQueue;
    std::mutex _recvMutex;
    std::function<bool(uint8_t)> _filter;
    MidiParser _midiParser;
};
