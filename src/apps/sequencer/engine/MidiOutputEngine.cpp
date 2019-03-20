#include "MidiOutputEngine.h"
#include "Engine.h"

#include "model/Model.h"
#include "model/MidiOutput.h"

#include "core/midi/MidiMessage.h"

MidiOutputEngine::MidiOutputEngine(Engine &engine, Model &model):
    _engine(engine),
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
            int(output.noteSource()) - int(MidiOutput::Output::NoteSource::FirstNote);

        int velocity = int(output.velocitySource()) <= int(MidiOutput::Output::VelocitySource::LastTrack) ?
            outputState.velocity :
            int(output.velocitySource()) - int(MidiOutput::Output::VelocitySource::FirstVelocity);

        if (outputState.hasRequest(OutputState::Slide)) {
            sendMidi(tick, port, MidiMessage::makeControlChange(channel, 65, outputState.slide ? 127 : 0));
        }

        if (outputState.hasRequest(OutputState::NoteOn)) {
            sendMidi(tick, port, MidiMessage::makeNoteOn(channel, note, velocity));
        }

        if (outputState.hasRequest(OutputState::NoteOff) && outputState.activeNote != -1) {
            sendMidi(tick, port, MidiMessage::makeNoteOff(channel, outputState.activeNote));
            outputState.activeNote = -1;
        }

        if (outputState.hasRequest(OutputState::NoteOn)) {
            outputState.activeNote = note;
        }

        if (outputState.hasRequest(OutputState::ControlChange)) {
            sendMidi(tick, port, MidiMessage::makeControlChange(channel, output.controlNumber(), outputState.control));
        }

        outputState.resetRequests();
    }
}

void MidiOutputEngine::sendGate(int trackIndex, bool gate) {
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        const auto &output = _midiOutput.output(outputIndex);
        auto &outputState = _outputStates[outputIndex];

        if (output.takesGateFromTrack(trackIndex)) {
            outputState.setRequest(gate ? OutputState::NoteOn : OutputState::NoteOff);
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
            outputState.setRequest(OutputState::ControlChange);
            outputState.control = clamp(int(std::floor(cv * (127.f / 5.f))), 0, 127);
        }
    }
}

void MidiOutputEngine::sendSlide(int trackIndex, bool slide) {
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        const auto &output = _midiOutput.output(outputIndex);
        auto &outputState = _outputStates[outputIndex];

        if (output.takesNoteFromTrack(trackIndex)) {
            outputState.setRequest(OutputState::Slide);
            outputState.slide = slide;
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
