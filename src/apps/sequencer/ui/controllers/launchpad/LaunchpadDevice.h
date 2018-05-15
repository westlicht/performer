#pragma once

#include "core/midi/MidiMessage.h"

#include <array>
#include <bitset>
#include <functional>

class LaunchpadDevice {
public:
    static constexpr int Rows = 8;
    static constexpr int Cols = 8;
    static constexpr int ExtraRows = 2;
    static constexpr int ButtonCount = (Rows + ExtraRows) * Cols;

    static constexpr int SceneRow = 8;
    static constexpr int FunctionRow = 9;

    typedef std::function<void(const MidiMessage &)> SendMidiHandler;
    typedef std::function<void(int, int, bool)> ButtonHandler;

    LaunchpadDevice();

    // midi handling

    void setSendMidiHandler(SendMidiHandler sendMidiHandler) {
        _sendMidiHandler = sendMidiHandler;
    }

    void recvMidi(const MidiMessage &message);

    // button handling

    void setButtonHandler(ButtonHandler buttonHandler) {
        _buttonHandler = buttonHandler;
    }

    bool buttonState(int row, int col) const {
        return _buttonState[row * Cols + col];
    }

    // led handling

    void clearLeds() {
        std::fill(_ledState.begin(), _ledState.end(), 0);
    }

    void setLed(int row, int col, int red, int green) {
        uint8_t state = (red & 0x3) | ((green & 0x3) << 4);
        _ledState[row * Cols + col] = state;
    }

    void syncLeds();

private:
    void sendMidi(const MidiMessage &message) {
        if (_sendMidiHandler) {
            _sendMidiHandler(message);
        }
    }

    void setButtonState(int row, int col, bool state) {
        _buttonState[row * Cols + col] = state;
        if (_buttonHandler) {
            _buttonHandler(row, col, state);
        }
    }

    SendMidiHandler _sendMidiHandler;
    ButtonHandler _buttonHandler;
    std::bitset<ButtonCount> _buttonState;
    std::array<uint8_t, ButtonCount> _ledState;
    std::array<uint8_t, ButtonCount> _deviceLedState;
};
