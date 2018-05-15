#include "LaunchpadDevice.h"

LaunchpadDevice::LaunchpadDevice()
{
    std::fill(_deviceLedState.begin(), _deviceLedState.end(), 0xff);
    clearLeds();
}

void LaunchpadDevice::recvMidi(const MidiMessage &message) {
    if (message.isNoteOn()) {
        int index = message.note();
        int row = index / 16;
        int col = index % 16;
        if (row < Rows && col < Cols) {
            setButtonState(row, col, message.velocity() != 0);
        } else if (row < Rows && col == Cols) {
            setButtonState(SceneRow, row, message.velocity() != 0);
        }
    } else if (message.isControlChange()) {
        int index = message.controlNumber();
        if (index >= 104 && index < 112) {
            setButtonState(FunctionRow, index - 104, message.controlValue() != 0);
        }
    }
}

void LaunchpadDevice::syncLeds() {
    // grid
    for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Cols; ++col) {
            int index = row * Cols + col;
            if (_deviceLedState[index] != _ledState[index]) {
                sendMidi(MidiMessage::makeNoteOn(0, row * 16 + col, _ledState[index]));
                _deviceLedState[index] = _ledState[index];
            }
        }
    }

    // scene
    for (int col = 0; col < Cols; ++col) {
        int index = SceneRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            sendMidi(MidiMessage::makeNoteOn(0, col * 16 + 8, _ledState[index]));
            _deviceLedState[index] = _ledState[index];
        }
    }

    // function
    for (int col = 0; col < Cols; ++col) {
        int index = FunctionRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            sendMidi(MidiMessage::makeControlChange(0, 104 + col, _ledState[index]));
            _deviceLedState[index] = _ledState[index];
        }
    }
}
