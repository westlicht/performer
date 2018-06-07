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

class UsbMidi {
public:
    typedef std::function<void(uint16_t vendorId, uint16_t productId)> ConnectHandler;
    typedef std::function<void()> DisconnectHandler;
    typedef std::function<bool(uint8_t)> RecvFilter;

    UsbMidi() :
        _simulator(sim::Simulator::instance())
    {
        _port = std::make_shared<sim::Midi::Port>(
            sim::usbMidiPortConfig.port,
            [this] (uint8_t data) {
                if (!_recvFilter || !_recvFilter(data)) {
                    std::lock_guard<std::mutex> lock(_recvMutex);
                    _recvQueue.emplace_back(data);
                }
            },
            [this] () {
                if (_connectHandler) {
                    _connectHandler(sim::usbMidiPortConfig.vendorId, sim::usbMidiPortConfig.productId);
                }
            },
            [this] () {
                if (_disconnectHandler) {
                    _disconnectHandler();
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

    void setConnectHandler(ConnectHandler handler) {
        _connectHandler = handler;
    }

    void setDisconnectHandler(DisconnectHandler handler) {
        _disconnectHandler = handler;
    }

    void setRecvFilter(RecvFilter filter) {
        _recvFilter = filter;
    }

private:
    ConnectHandler _connectHandler;
    DisconnectHandler _disconnectHandler;
    RecvFilter _recvFilter;

    sim::Simulator &_simulator;
    std::shared_ptr<sim::Midi::Port> _port;
    std::deque<uint8_t> _recvQueue;
    std::mutex _recvMutex;
    MidiParser _midiParser;
};
