#pragma once

#include <cstdint>

namespace sim {

struct MidiConfig {
    const char *portIn;
    const char *portOut;
    uint16_t vendorId;
    uint16_t productId;
};

static const MidiConfig midiPortConfig = {
    .portIn = "Launchkey Mini LK Mini MIDI",
    .portOut = "Launchkey Mini LK Mini MIDI"
};

static const MidiConfig usbMidiPortConfig = {
    .portIn = "Launchpad Mini 2",
    .portOut = "Launchpad Mini 2",
    .vendorId = 0x1235,
    .productId = 0x0037
};

} // namespace sim
