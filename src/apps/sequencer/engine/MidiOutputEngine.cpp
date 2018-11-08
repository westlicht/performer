#include "MidiOutputEngine.h"
#include "Engine.h"

#include "model/Model.h"
#include "model/MidiOutput.h"

#include "core/midi/MidiMessage.h"

MidiOutputEngine::MidiOutputEngine(Engine &engine, Model &model):
    _engine(engine),
    _model(model),
    _midiOutput(model.project().midiOutput())
{
    reset();
}

void MidiOutputEngine::reset() {
    for (auto &outputState : _outputStates) {
        resetActiveNote(0, outputState);
        outputState.reset();
    }
}

void MidiOutputEngine::tick(uint32_t tick) {
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        const auto &output = _midiOutput.output(outputIndex);
        auto &outputState = _outputStates[outputIndex];

        // check if target has changed
        if (outputState.activeTarget != output.target()) {
            resetActiveNote(tick, outputState);
            outputState.reset();
            outputState.activeTarget = output.target();
        }

        if (!outputState.requests) {
            continue;
        }

        MidiPort port = MidiPort(output.target().port());
        int channel = output.target().channel();

        int note = int(output.noteSource()) <= int(MidiOutput::Output::NoteSource::LastTrack) ?
            outputState.note :
            int(output.noteSource()) - int(MidiOutput::Output::NoteSource::LastTrack);

        int velocity = int(output.velocitySource()) <= int(MidiOutput::Output::VelocitySource::LastTrack) ?
            outputState.velocity :
            int(output.velocitySource()) - int(MidiOutput::Output::VelocitySource::LastTrack);

        bool noteOnRequest = outputState.requests & MidiOutputEngine::OutputState::NoteOn;
        bool noteOffRequest = outputState.requests & MidiOutputEngine::OutputState::NoteOff;
        bool controlChangeRequest = outputState.requests & MidiOutputEngine::OutputState::ControlChange;

        if (noteOffRequest && !noteOnRequest) {
            sendMidi(tick, port, MidiMessage::makeNoteOff(channel, note));
            if (outputState.activeNote == note) {
                outputState.activeNote = -1;
            }
        }

        if (noteOnRequest && !noteOffRequest) {
            resetActiveNote(tick, outputState);
            sendMidi(tick, port, MidiMessage::makeNoteOn(channel, note, velocity));
            outputState.activeNote = note;
        }

        if (controlChangeRequest) {
            sendMidi(tick, port, MidiMessage::makeControlChange(channel, output.controlNumber(), outputState.control));
        }

        outputState.requests = 0;
    }
}

void MidiOutputEngine::sendGate(int trackIndex, bool gate) {
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        const auto &output = _midiOutput.output(outputIndex);
        auto &outputState = _outputStates[outputIndex];

        if (output.takesGateFromTrack(trackIndex)) {
            if (gate) {
                outputState.requests |= MidiOutputEngine::OutputState::NoteOn;
            } else {
                outputState.requests |= MidiOutputEngine::OutputState::NoteOff;
            }
        }
    }
}

void MidiOutputEngine::sendCv(int trackIndex, float cv) {
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        const auto &output = _midiOutput.output(outputIndex);
        auto &outputState = _outputStates[outputIndex];

        if (output.takesNoteFromTrack(trackIndex)) {
            outputState.note = clamp(60 + int(std::floor(cv * 12.f + 0.01f)), 0, 127);
        }

        if (output.takesVelocityFromTrack(trackIndex)) {
            outputState.velocity = clamp(int(std::floor(cv * (127.f / 5.f))), 0, 127);
        }

        if (output.takesControlFromTrack(trackIndex)) {
            outputState.requests |= MidiOutputEngine::OutputState::ControlChange;
            outputState.control = clamp(int(std::floor(cv * (127.f / 5.f))), 0, 127);
        }
    }
}

void MidiOutputEngine::resetActiveNote(uint32_t tick, OutputState &outputState) {
    if (outputState.activeNote >= 0) {
        MidiPort port = MidiPort(outputState.activeTarget.port());
        int channel = outputState.activeTarget.channel();
        sendMidi(tick, port, MidiMessage::makeNoteOff(channel, outputState.activeNote));
        outputState.activeNote = -1;
    }
}

void MidiOutputEngine::sendMidi(uint32_t tick, MidiPort port, const MidiMessage &message) {
    // DBG("tick=%d ", tick);
    // MidiMessage::dump(message);
    _engine.sendMidi(port, message);
}
