#pragma once

#include "core/utils/RingBuffer.h"
#include "core/midi/MidiMessage.h"

#include <functional>

#include <cstdint>

class UsbMidi {
public:
    typedef std::function<void(uint16_t vendorId, uint16_t productId)> ConnectHandler;
    typedef std::function<void()> DisconnectHandler;
    typedef std::function<bool(uint8_t)> RecvFilter;

    void init() {}

    bool send(const MidiMessage &message) {
        if (_txQueue.full()) {
            return false;
        }
        _txQueue.write(message);
        return true;
    }

    bool recv(MidiMessage *message) {
        if (_rxQueue.empty()) {
            return false;
        }
        *message = _rxQueue.read();
        return true;
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
    void connect(uint16_t vendorId, uint16_t productId) {
        if (_connectHandler) {
            _connectHandler(vendorId, productId);
        }
    }

    void disconnect() {
        if (_disconnectHandler) {
            _disconnectHandler();
        }
    }

    void enqueueMessage(MidiMessage &message) {
        _rxQueue.write(message);
    }

    void enqueueData(uint8_t data) {
        if (_recvFilter && !_recvFilter(data)) {
            // _recvFilter(data);
        }
    }

    bool dequeueMessage(MidiMessage *message) {
        if (_txQueue.empty()) {
            return false;
        }
        *message = _txQueue.read();
        return true;
    }

    ConnectHandler _connectHandler;
    DisconnectHandler _disconnectHandler;
    RecvFilter _recvFilter;

    RingBuffer<MidiMessage, 128> _txQueue;
    RingBuffer<MidiMessage, 16> _rxQueue;

    friend class UsbH;
};
