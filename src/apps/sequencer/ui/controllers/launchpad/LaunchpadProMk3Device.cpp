#include "LaunchpadProMk3Device.h"

//         +---+---+---+---+---+---+---+---+
//         | 91| 92| 93| 94| 95| 96| 97| 98| < CC messages
//         +---+---+---+---+---+---+---+---+
//    v CC messages                 CC messages v
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 80|  | 81|...|   |   |   |   |   | 88|  | 89|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 70|  | 71|...|   |   |   |   |   | 78|  | 79|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 60|  | 61|...|   |   |   |   |   | 68|  | 69|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 50|  | 51|...|   |   |   |   |   | 58|  | 59|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 40|  | 41|...|   |   |   |   |   | 48|  | 49|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 30|  | 31|...|   |   |   |   |   | 38|  | 39|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 20|  | 21|...|   |   |   |   |   | 28|  | 29|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 10|  | 11|...|   |   |   |   |   | 18|  | 19|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//
//         +---+---+---+---+---+---+---+---+
//         |  1|  2|  3|  4|  5|  6|  7|  8| < CC messages
//         +---+---+---+---+---+---+---+---+


LaunchpadProMk3Device::LaunchpadProMk3Device() :
    LaunchpadDevice()
{
}

void LaunchpadProMk3Device::initialize() {
    // send sysex message to enter programmer mode
    std::array<uint8_t, 7> payload { 0x00, 0x20, 0x29, 0x02, 0x0e, 0x0e, 0x01 };
    sendMidi(Cable, MidiMessage::makeSystemExclusive(payload.data(), payload.size()));
}

void LaunchpadProMk3Device::recvMidi(uint8_t cable, const MidiMessage &message) {
    if (message.isNoteOn() || message.isNoteOff()) {
        int index = message.note();
        int row = 7 - (index - 11) / 10;
        int col = (index - 11) % 10;
        if (row >= 0 && row < Rows && col >= 0 && col < Cols) {
            setButtonState(row, col, message.velocity() != 0);
        }
    } else if (message.isControlChange()) {
        int index = message.controlNumber();
        if (index >= 91 && index <= 98) {
            setButtonState(FunctionRow, index - 91, message.controlValue() != 0);
        } else if (index >= 19 && index <= 89 && (index % 10) == 9) {
            setButtonState(SceneRow, 7 - (index - 19) / 10, message.controlValue() != 0);
        }
    }
}

void LaunchpadProMk3Device::syncLeds() {
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
            if (sendMidi(Cable, MidiMessage::makeControlChange(0, 11 + 10 * (7 - col) + 8, _ledState[index]))) {
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
