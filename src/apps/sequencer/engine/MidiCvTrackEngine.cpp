#include "MidiCvTrackEngine.h"

void MidiCvTrackEngine::reset() {
}

void MidiCvTrackEngine::tick(uint32_t tick) {
    _pitchBend = 0.f;
    _cvOutput = 0.f;
}

void MidiCvTrackEngine::receiveMidi(MidiPort port, int channel, const MidiMessage &message) {
    if (port == MidiPort(_midiCvTrack.port()) && (_midiCvTrack.channel() == -1 || _midiCvTrack.channel() == channel)) {
        if (message.isNoteOn()) {

        } else if (message.isNoteOff()) {

        } else if (message.isPitchBend()) {
            _pitchBend = message.pitchBend() * (1.f / 8192.f);
        }
    }
}
