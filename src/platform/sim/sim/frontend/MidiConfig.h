#pragma once

#include <cstdint>

namespace sim {

struct MidiConfig {
    const char *port;
    const char *portIn;
    const char *portOut;
    uint16_t vendorId;
    uint16_t productId;
};

static const MidiConfig midiPortConfig = {
    // .port = "SL MkII Port 1"
    .port = "Launchkey Mini LK Mini MIDI"
};

static const MidiConfig usbMidiPortConfig = {
    .port = "Launchpad Mini 2",
    .vendorId = 0x1235,
    .productId = 0x0037
};

} // namespace sim
