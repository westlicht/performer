#pragma once

#include <cstdint>

enum class MidiPort : uint8_t {
    Midi,
    UsbMidi,
    CvGate,
};

static const char *midiPortName(MidiPort port) {
    switch (port) {
    case MidiPort::Midi:    return "MIDI";
    case MidiPort::UsbMidi: return "USB";
    case MidiPort::CvGate:  return "CV/GATE";
    }
    return nullptr;
}
