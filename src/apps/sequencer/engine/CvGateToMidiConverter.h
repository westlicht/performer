#pragma once

#include "core/midi/MidiMessage.h"
#include "core/math/Math.h"

#include <functional>

class CvGateToMidiConverter {
public:
    CvGateToMidiConverter() {
        reset();
    }

    void reset() {
        _lastGateOff = os::ticks();
        _gate = 0;
        _note = -1;
    }

    void convert(float pitchCv, float gateCv, uint8_t channel, std::function<void(const MidiMessage &message)> callback) {
        int8_t note = clamp(60 + int(std::floor(pitchCv * 12.f + 0.5f)), 0, 127);

        if (_gate) {
            if (gateCv < 2.f) {
                // gate off
                if (_note != -1) {
                    callback(MidiMessage::makeNoteOff(channel, _note, 0));
                    _note = -1;
                }
                _gate = 0;
                _lastGateOff = os::ticks();
            } else if (note != _note) {
                // legato note change
                callback(MidiMessage::makeNoteOn(channel, note, 127));
                callback(MidiMessage::makeNoteOff(channel, _note, 0));
                _note = note;
            }
        } else {
            if (gateCv > 3.f) {
                if (os::ticks() - _lastGateOff >= GateOnDelay) {
                    // gate on
                    callback(MidiMessage::makeNoteOn(channel, note, 127));
                    _gate = 1;
                    _note = note;
                }
            } else {
                _lastGateOff = os::ticks();
            }
        }
    }

private:
    static constexpr uint32_t GateOnDelay = os::time::ms(5);

    uint32_t _lastGateOff;
    uint8_t _gate;
    int8_t _note;
};
