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

    bool send(uint8_t cable, const MidiMessage &message) {
        if (_txQueue.full()) {
            return false;
        }
        _txQueue.write({ cable, message });
        return true;
    }

    bool recv(uint8_t *cable, MidiMessage *message) {
        if (_rxQueue.empty()) {
            return false;
        }
        auto cableAndMessage = _rxQueue.read();
        *cable = cableAndMessage.cable;
        *message = cableAndMessage.message;
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

    uint32_t rxOverflow() const { return 0; }

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

    void enqueueMessage(uint8_t cable, const MidiMessage &message) {
        if (_rxQueue.full()) {
            // overflow
            ++_rxOverflow;
        }
        _rxQueue.write({ cable, message });
    }

    void enqueueData(uint8_t cable, uint8_t data) {
        if (_recvFilter && !_recvFilter(data)) {
            // _recvFilter(data);
        }
    }

    bool dequeueMessage(uint8_t *cable, MidiMessage *message) {
        if (_txQueue.empty()) {
            return false;
        }
        auto messageAndCable = _txQueue.readAndReplace();
        *cable = messageAndCable.cable;
        *message = messageAndCable.message;
        return true;
    }

    ConnectHandler _connectHandler;
    DisconnectHandler _disconnectHandler;
    RecvFilter _recvFilter;

    struct CableAndMessage {
        uint8_t cable;
        MidiMessage message;
    };

    RingBuffer<CableAndMessage, 128> _txQueue;
    RingBuffer<CableAndMessage, 16> _rxQueue;
    volatile uint32_t _rxOverflow = 0;

    friend class UsbH;
};
