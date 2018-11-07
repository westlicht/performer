#include "MidiOutputEngine.h"
#include "Engine.h"

#include "model/Model.h"
#include "model/MidiOutput.h"

MidiOutputEngine::MidiOutputEngine(Engine &engine, Model &model):
    _engine(engine),
    _model(model),
    _midiOutput(model.project().midiOutput())
{
}

void MidiOutputEngine::reset() {
}

void MidiOutputEngine::tick(uint32_t tick) {
}

void MidiOutputEngine::sendGate(int trackIndex, bool gate) {
    for (const auto &output : _midiOutput.outputs()) {
        if (output.takesGateFromTrack(trackIndex)) {
        }
    }
}

void MidiOutputEngine::sendCv(int trackIndex, float cv) {
    for (const auto &output : _midiOutput.outputs()) {
        if (output.takesNoteFromTrack(trackIndex)) {

        }
        if (output.takesVelocityFromTrack(trackIndex)) {

        }
        if (output.takesControlFromTrack(trackIndex)) {

        }
    }
}
