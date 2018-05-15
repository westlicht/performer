#include "LaunchpadController.h"

#include "core/Debug.h"


static void drawNoteSequence(LaunchpadDevice &device, const NoteSequence &sequence, int currentStep) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int stepIndex = row * 8 + col;
            const auto &step = sequence.step(stepIndex);
            device.setLed(row, col, stepIndex == currentStep, step.gate() ? 1 : 0);
        }
    }
}

LaunchpadController::LaunchpadController(ControllerManager &manager, Model &model, Engine &engine) :
    Controller(manager, model, engine)
{
    _device.setSendMidiHandler([this] (const MidiMessage &message) {
        sendMidi(message);
        // MidiMessage::dump(message);
    });

    _device.setButtonHandler([this] (int row, int col, bool state) {
        DBG("button %d/%d - %d", row, col, state);
        if (state) {
            buttonDown(row, col);
        } else {
            buttonUp(row, col);
        }
    });
}

void LaunchpadController::update() {
    const auto &project = _model.project();

    _device.clearLeds();
    _device.setLed(LaunchpadDevice::FunctionRow, project.selectedTrackIndex(), 1, 1);
    _device.setLed(LaunchpadDevice::SceneRow, project.selectedTrackIndex(), 1, 1);

    if (project.selectedTrack().trackMode() == Track::TrackMode::Note) {
        const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
        const auto &sequence = project.selectedNoteSequence();
        int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;
        drawNoteSequence(_device, sequence, currentStep);

    }

    _device.syncLeds();
}

void LaunchpadController::recvMidi(const MidiMessage &message) {
    _device.recvMidi(message);
    // MidiMessage::dump(message);
}

void LaunchpadController::buttonDown(int row, int col) {
    if (row == LaunchpadDevice::FunctionRow) {
        _model.project().setSelectedTrackIndex(col);
    }

}

void LaunchpadController::buttonUp(int row, int col) {

}
