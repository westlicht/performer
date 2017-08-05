#pragma once

#include "Config.h"

#include <array>

namespace KeyCode {

    static constexpr int Step(int step) {
        return step < 8 ? (8 + step) : (step - 8);
    }

    static constexpr int Track(int track) {
        return 16 + track;
    }

    static constexpr int Function(int function) {
        return 32 + function;
    }

} // namespace KeyCode

typedef std::array<bool, CONFIG_BLM_ROWS * CONFIG_BLM_COLS_BUTTON> KeyState;

class Key {
public:
    // key codes
    enum Code {
        Track0 = KeyCode::Track(0),
        Track1 = KeyCode::Track(1),
        Track2 = KeyCode::Track(2),
        Track3 = KeyCode::Track(3),
        Track4 = KeyCode::Track(4),
        Track5 = KeyCode::Track(5),
        Track6 = KeyCode::Track(6),
        Track7 = KeyCode::Track(7),
        Step0 = KeyCode::Step(0),
        Step1 = KeyCode::Step(1),
        Step2 = KeyCode::Step(2),
        Step3 = KeyCode::Step(3),
        Step4 = KeyCode::Step(4),
        Step5 = KeyCode::Step(5),
        Step6 = KeyCode::Step(6),
        Step7 = KeyCode::Step(7),
        Step8 = KeyCode::Step(8),
        Step9 = KeyCode::Step(9),
        Step10 = KeyCode::Step(10),
        Step11 = KeyCode::Step(11),
        Step12 = KeyCode::Step(12),
        Step13 = KeyCode::Step(13),
        Step14 = KeyCode::Step(14),
        Step15 = KeyCode::Step(15),
        Start = 24,
        BPM = 25,
        Mute = 26,
        Shift = 31,
        F0 = KeyCode::Function(0),
        F1 = KeyCode::Function(1),
        F2 = KeyCode::Function(2),
        F3 = KeyCode::Function(3),
        F4 = KeyCode::Function(4),
    };

    Key(int code, const KeyState &state) : _code(code), _state(state) {}

    int code() const { return _code; }

    const KeyState &state() const { return _state; }

    bool shiftModifier() const { return _state[Shift]; }

    bool is(int code) const { return _code == code; }

    bool isTrack() const { return _code >= Track0 && _code <= Track7; }
    int track() const { return _code - Track0; }

    bool isStep() const { return _code >= Step8 && _code <= Step7; }
    int step() const { return _code >= Step0 ? _code - Step0 : _code - Step8 + 8; }

    bool isFunction() const { return _code >= F0 && _code <= F4; }
    int function() const { return _code - F0; }

    bool isGlobal() const { return _code == Start; };

private:
    int _code;
    const KeyState &_state;
};
