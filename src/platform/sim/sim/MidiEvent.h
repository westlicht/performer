#pragma once

#include "core/midi/MidiMessage.h"

#include <cstdint>

namespace sim {

struct MidiEvent {
    enum Kind {
        Connect,
        Disconnect,
        Message,
    };

    int kind;
    int port;
    MidiMessage message;
    struct {
        uint16_t vendorId;
        uint16_t productId;
    } connect;

    MidiEvent() : message() {}
    MidiEvent(Kind kind, int port) : kind(kind), port(port) {}
    MidiEvent(const MidiEvent &other) = default;

    MidiEvent &operator=(const MidiEvent &other) = default;

    static MidiEvent makeConnect(int port, uint16_t vendorId, uint16_t productId) {
        MidiEvent event(Connect, port);
        event.connect = { vendorId, productId };
        return event;
    }

    static MidiEvent makeDisconnect(int port) {
        MidiEvent event(Disconnect, port);
        return event;
    }

    static MidiEvent makeMessage(int port, MidiMessage message) {
        MidiEvent event(Message, port);
        event.message = message;
        return event;
    }
};

} // namespace sim
