#pragma once

#include "Config.h"

#include "MidiPort.h"

#include "model/MidiConfig.h"
#include "model/MidiOutput.h"

#include <array>
#include <cstdint>

class Engine;
class Model;
class MidiMessage;

class MidiOutputEngine {
public:
    MidiOutputEngine(Engine &engine, Model &model);

    void reset();
    void update(bool forceSendCC = false);

    void sendGate(int trackIndex, bool gate);
    void sendSlide(int trackIndex, bool slide);
    void sendCv(int trackIndex, float cv);
    void sendProgramChange(int channel, int programNumber);

    // Malekko integration
    void sendMalekkoSelectHandshake(int channel);
    void sendMalekkoSelectReleaseHandshake(int channel);
    void sendMalekkoSaveHandshake(int channel);

private:
    struct OutputState {
        enum Requests {
            NoteOn          = 1<<0,
            NoteOff         = 1<<1,
            ControlChange   = 1<<2,
            Slide           = 1<<3,
        };

        MidiOutput::Output::Event event = MidiOutput::Output::Event::None;
        MidiTargetConfig target;

        uint8_t requests;
        int8_t note;
        int8_t slide;
        int8_t velocity;
        int8_t control;

        int8_t activeNote;

        OutputState() { reset(); }

        void reset() {
            requests = 0;
            note = 60;
            slide = 0;
            velocity = 100;
            control = 0;

            activeNote = -1;
        };

        void setRequest(uint8_t request) { requests |= request; }
        void clearRequest(uint8_t request) { requests &= ~request; }
        bool hasRequest(uint8_t request) { return requests & request; }
    };

    void resetOutput(int outputIndex);

    void sendMidi(MidiPort port, const MidiMessage &message);

    Engine &_engine;
    const MidiOutput &_midiOutput;
    std::array<OutputState, CONFIG_MIDI_OUTPUT_COUNT> _outputStates;
    uint32_t _lastSendCCTicks = 0;
};
