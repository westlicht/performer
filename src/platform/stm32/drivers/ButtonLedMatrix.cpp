#include "ButtonLedMatrix.h"

#include "core/profiler/Profiler.h"
#include "core/Debug.h"

#include <cstring>

ButtonLedMatrix::ButtonLedMatrix(ShiftRegister &shiftRegister, bool invertLeds) :
    _shiftRegister(shiftRegister),
    _invertLeds(invertLeds)
{
}

void ButtonLedMatrix::init() {
    std::memset(_buttonState, 0, sizeof(_buttonState));
    std::memset(_ledState, 0, sizeof(_ledState));
}

void ButtonLedMatrix::process() {
    uint8_t rowData = ~(1 << _row);
    uint8_t ledData = 0;
    for (int col = 0; col < ColsLed; ++col) {
        int index = col * Rows + _row;
        if (_ledState[index].red.update()) {
            ledData |= (1 << (col * 2 + 1));
        }
        if (_ledState[index].green.update()) {
            ledData |= (1 << (col * 2));
        }
    }

    uint8_t buttonData = _shiftRegister.read(0);
    _shiftRegister.write(0, rowData);
    _shiftRegister.write(1, ledData);

    static bool first = true;
    if (first) {
        first = false;
    } else {
        uint8_t scanRow = (_row + 6) % Rows;
        for (int col = 0; col < ColsButton; ++col) {
            int buttonIndex = col * Rows + scanRow;
            auto &state = _buttonState[buttonIndex].state;
            bool newState = !(buttonData & (1 << col));
            if (newState != state) {
                state = newState;
                _events.write(Event(state ? Event::KeyDown : Event::KeyUp, buttonIndex));
            }
        }
    }

    _row = (_row + 1) % Rows;
}
