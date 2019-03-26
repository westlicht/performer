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
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        resetOutput(outputIndex);
    }
}

void MidiOutputEngine::tick(uint32_t tick) {
    for (int outputIndex = 0; outputIndex < CONFIG_TRACK_COUNT; ++outputIndex) {
        const auto &output = _midiOutput.output(outputIndex);
        auto &outputState = _outputStates[outputIndex];

        // check if event or target has changed
        if (outputState.event != output.event() || outputState.target != output.target()) {
            resetOutput(outputIndex);
            outputState.event = output.event();
            outputState.target = output.target();
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
            sendMidi(port, MidiMessage::makeControlChange(channel, 65, outputState.slide ? 127 : 0));
        }

        if (outputState.hasRequest(OutputState::NoteOn)) {
            sendMidi(port, MidiMessage::makeNoteOn(channel, note, velocity));
        }

        if (outputState.hasRequest(OutputState::NoteOff) && outputState.activeNote != -1) {
            sendMidi(port, MidiMessage::makeNoteOff(channel, outputState.activeNote));
            outputState.activeNote = -1;
        }

        if (outputState.hasRequest(OutputState::NoteOn)) {
            outputState.activeNote = note;
        }

        if (outputState.hasRequest(OutputState::ControlChange)) {
            sendMidi(port, MidiMessage::makeControlChange(channel, output.controlNumber(), outputState.control));
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

void MidiOutputEngine::resetOutput(int outputIndex) {
    auto &outputState = _outputStates[outputIndex];

    MidiPort port = MidiPort(outputState.target.port());
    int channel = outputState.target.channel();

    if (outputState.activeNote >= 0) {
        sendMidi(port, MidiMessage::makeNoteOff(channel, outputState.activeNote));
    }

    if (outputState.event == MidiOutput::Output::Event::Note) {
        // all sound off
        sendMidi(port, MidiMessage::makeControlChange(channel, 120, 0));
    }

    outputState.reset();
}

void MidiOutputEngine::sendMidi(MidiPort port, const MidiMessage &message) {
    // MidiMessage::dump(message);
    _engine.sendMidi(port, message);
}
