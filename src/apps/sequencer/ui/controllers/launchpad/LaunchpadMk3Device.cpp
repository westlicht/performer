#include "LaunchpadMk3Device.h"

//  +---+---+---+---+---+---+---+---+
//  | 91| 92| 93| 94| 95| 96| 97| 98| < CC messages
//  +---+---+---+---+---+---+---+---+
//                           CC messages v
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

LaunchpadMk3Device::LaunchpadMk3Device() :
    LaunchpadDevice()
{
}

void LaunchpadMk3Device::initialize() {
    // send sysex message to enter programmer mode
    std::array<uint8_t, 7> payload { 0x00, 0x20, 0x29, 0x02, 0x0d, 0x0e, 0x01 };
    sendMidi(Cable, MidiMessage::makeSystemExclusive(payload.data(), payload.size()));
}

void LaunchpadMk3Device::recvMidi(uint8_t cable, const MidiMessage &message) {
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
        if (index >= 91 && index <= 98) {
            setButtonState(FunctionRow, index - 91, message.controlValue() != 0);
        } else if (index >= 19 && index <= 89 && index % 10 == 9) {
            setButtonState(SceneRow, 7 - (index - 19) / 10, message.controlValue() != 0);
        }
    }
}

void LaunchpadMk3Device::syncLeds() {
    // grid
    for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Cols; ++col) {
            int index = row * Cols + col;
            if (_deviceLedState[index] != _ledState[index]) {
                if (sendMidi(Cable, MidiMessage::makeNoteOn(0, 11 + 10 * (7 - row) + col, _ledState[index]))) {
                    _deviceLedState[index] = _ledState[index];
                }
            }
        }
    }

    // scene
    for (int col = 0; col < Cols; ++col) {
        int index = SceneRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            if (sendMidi(Cable, MidiMessage::makeControlChange(0, 19 + 10 * (7 - col), _ledState[index]))) {
                _deviceLedState[index] = _ledState[index];
            }
        }
    }

    // function
    for (int col = 0; col < Cols; ++col) {
        int index = FunctionRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            if (sendMidi(Cable, MidiMessage::makeControlChange(0, 91 + col, _ledState[index]))) {
                _deviceLedState[index] = _ledState[index];
            }
        }
    }
}
