#include "LaunchpadMk2Device.h"

//  +---+---+---+---+---+---+---+---+
//  |104|105|106|107|108|109|110|111| < CC messages
//  +---+---+---+---+---+---+---+---+
//
//  +---+---+---+---+---+---+---+---+  +---+
//  | 81|...|   |   |   |   |   | 88|  | 89|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 71|...|   |   |   |   |   | 78|  | 79|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 61|...|   |   |   |   |   | 68|  | 69|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 51|...|   |   |   |   |   | 58|  | 59|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 41|...|   |   |   |   |   | 48|  | 49|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 31|...|   |   |   |   |   | 38|  | 39|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 21|...|   |   |   |   |   | 28|  | 29|
//  +---+---+---+---+---+---+---+---+  +---+
//  | 11|...|   |   |   |   |   | 18|  | 19|
//  +---+---+---+---+---+---+---+---+  +---+

LaunchpadMk2Device::LaunchpadMk2Device() :
    LaunchpadDevice()
{
}

void LaunchpadMk2Device::recvMidi(const MidiMessage &message) {
    if (message.isNoteOn() || message.isNoteOff()) {
        int index = message.note();
        int row = 7 - (index - 11) / 10;
        int col = (index - 11) % 10;
        if (row >= 0 && row < Rows && col >= 0 && col < Cols) {
            setButtonState(row, col, message.velocity() != 0);
        } else if (row >= 0 && row < Rows && col == Cols) {
            setButtonState(SceneRow, row, message.velocity() != 0);
        }
    } else if (message.isControlChange()) {
        int index = message.controlNumber();
        if (index >= 104 && index < 112) {
            setButtonState(FunctionRow, index - 104, message.controlValue() != 0);
        }
    }
}

void LaunchpadMk2Device::syncLeds() {
    // grid
    for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Cols; ++col) {
            int index = row * Cols + col;
            if (_deviceLedState[index] != _ledState[index]) {
                if (sendMidi(MidiMessage::makeNoteOn(0, 11 + 10 * (7 - row) + col, _ledState[index]))) {
                    _deviceLedState[index] = _ledState[index];
                }
            }
        }
    }

    // scene
    for (int col = 0; col < Cols; ++col) {
        int index = SceneRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            if (sendMidi(MidiMessage::makeNoteOn(0, 11 + 10 * (7 - col) + 8, _ledState[index]))) {
                _deviceLedState[index] = _ledState[index];
            }
        }
    }

    // function
    for (int col = 0; col < Cols; ++col) {
        int index = FunctionRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            if (sendMidi(MidiMessage::makeControlChange(0, 104 + col, _ledState[index]))) {
                _deviceLedState[index] = _ledState[index];
            }
        }
    }
}
