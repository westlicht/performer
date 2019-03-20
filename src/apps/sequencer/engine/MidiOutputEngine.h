#pragma once

#include "Config.h"

#include "MidiPort.h"

#include "model/MidiConfig.h"

#include <array>
#include <cstdint>

class Engine;
class Model;
class MidiOutput;
class MidiMessage;

class MidiOutputEngine {
public:
    MidiOutputEngine(Engine &engine, Model &model);

    void reset();
    void tick(uint32_t tick);

    void sendGate(int trackIndex, bool gate);
    void sendCv(int trackIndex, float cv);
    void sendSlide(int trackIndex, bool slide);

private:
    struct OutputState {
        enum Requests {
            NoteOn          = 1<<0,
            NoteOff         = 1<<1,
            ControlChange   = 1<<2,
            Slide           = 1<<3,
        };

        MidiTargetConfig activeTarget;
        int8_t activeNote = -1;

        uint8_t requests;
        int8_t note;
        int8_t slide;
        int8_t velocity;
        int8_t control;

        void reset() {
            requests = 0;
            note = 60;
            slide = 0;
            velocity = 100;
            control = 0;
        };

        void resetRequests() { requests = 0; }
        void setRequest(Requests request) { requests |= request; }
        bool hasRequest(Requests request) { return requests & request; }
    };

    void resetActiveNote(uint32_t tick, OutputState &outputState);

    void sendMidi(uint32_t tick, MidiPort port, const MidiMessage &message);

    Engine &_engine;
    const MidiOutput &_midiOutput;
    std::array<OutputState, CONFIG_TRACK_COUNT> _outputStates;
};
