#pragma once

#include "core/midi/MidiMessage.h"

#include <array>
#include <bitset>
#include <functional>

// Compatible with: Launchpad S, Launchpad Mini Mk1 and Mk2
class LaunchpadDevice {
public:
    static constexpr int Rows = 8;
    static constexpr int Cols = 8;
    static constexpr int ExtraRows = 2;
    static constexpr int ButtonCount = (Rows + ExtraRows) * Cols;

    static constexpr int SceneRow = 8;
    static constexpr int FunctionRow = 9;

    typedef std::function<bool(const MidiMessage &)> SendMidiHandler;
    typedef std::function<void(int, int, bool)> ButtonHandler;

    struct Color {
        union {
            struct {
                uint8_t red: 4;
                uint8_t green: 4;
            };
            uint8_t data;
        };

        Color(int red, int green) : red(red), green(green) {}
    };

    LaunchpadDevice();

    // midi handling

    void setSendMidiHandler(SendMidiHandler sendMidiHandler) {
        _sendMidiHandler = sendMidiHandler;
    }

    virtual void recvMidi(const MidiMessage &message);

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

    virtual void setLed(int row, int col, Color color) {
        _ledState[row * Cols + col] = color.data;
    }

    virtual void setLed(int row, int col, int red, int green) {
        uint8_t state = (red & 0x3) | ((green & 0x3) << 4);
        _ledState[row * Cols + col] = state;
    }

    virtual void syncLeds();

protected:
    bool sendMidi(const MidiMessage &message) {
        if (_sendMidiHandler) {
            return _sendMidiHandler(message);
        }
        return false;
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
