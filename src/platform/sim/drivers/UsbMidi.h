#pragma once

#include "core/midi/MidiMessage.h"

#include "sim/Simulator.h"

#include <functional>
#include <deque>
#include <memory>

#include <cstdint>

class UsbMidi : private sim::TargetInputHandler {
public:
    typedef std::function<void(uint16_t vendorId, uint16_t productId)> ConnectHandler;
    typedef std::function<void()> DisconnectHandler;
    typedef std::function<bool(uint8_t)> RecvFilter;

    UsbMidi() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    void init() {}

    bool send(uint8_t cable, const MidiMessage &message) {
        _simulator.writeMidiOutput(sim::MidiEvent::makeMessage(1, message));
        return true;
    }

    bool recv(uint8_t *cable, MidiMessage *message) {
        if (!_recvQueue.empty()) {
            *cable = 0;
            *message = _recvQueue.front();
            _recvQueue.pop_front();
            return true;
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

    uint32_t rxOverflow() const { return 0; }

private:
    void writeMidiInput(sim::MidiEvent event) {
        if (event.port == 1) {
            switch (event.kind) {
            case sim::MidiEvent::Connect:
                if (_connectHandler) {
                    _connectHandler(event.connect.vendorId, event.connect.productId);
                }
                break;
            case sim::MidiEvent::Disconnect:
                if (_disconnectHandler) {
                    _disconnectHandler();
                }
                break;
            case sim::MidiEvent::Message:
                if (event.message.length() != 1 || !_recvFilter || !_recvFilter(event.message.status())) {
                    _recvQueue.emplace_back(event.message);
                }
                break;
            }
        }
    }

    ConnectHandler _connectHandler;
    DisconnectHandler _disconnectHandler;
    RecvFilter _recvFilter;

    sim::Simulator &_simulator;
    std::deque<MidiMessage> _recvQueue;
};
