#pragma once

#include "SystemConfig.h"

#include "MatrixMap.h"

#include <array>

typedef std::array<bool, CONFIG_BLM_ROWS * CONFIG_BLM_COLS_BUTTON + 1> KeyState;

class Key {
public:
    // key codes
    enum Code {
        None = -1,
        Track0 = MatrixMap::fromTrack(0),
        Track1 = MatrixMap::fromTrack(1),
        Track2 = MatrixMap::fromTrack(2),
        Track3 = MatrixMap::fromTrack(3),
        Track4 = MatrixMap::fromTrack(4),
        Track5 = MatrixMap::fromTrack(5),
        Track6 = MatrixMap::fromTrack(6),
        Track7 = MatrixMap::fromTrack(7),
        Step0 = MatrixMap::fromStep(0),
        Step1 = MatrixMap::fromStep(1),
        Step2 = MatrixMap::fromStep(2),
        Step3 = MatrixMap::fromStep(3),
        Step4 = MatrixMap::fromStep(4),
        Step5 = MatrixMap::fromStep(5),
        Step6 = MatrixMap::fromStep(6),
        Step7 = MatrixMap::fromStep(7),
        Step8 = MatrixMap::fromStep(8),
        Step9 = MatrixMap::fromStep(9),
        Step10 = MatrixMap::fromStep(10),
        Step11 = MatrixMap::fromStep(11),
        Step12 = MatrixMap::fromStep(12),
        Step13 = MatrixMap::fromStep(13),
        Step14 = MatrixMap::fromStep(14),
        Step15 = MatrixMap::fromStep(15),
        Start = 24,
        BPM = 25,
        Mute = 26,
        Left = 28,
        Right = 29,
        Page = 30,
        Shift = 31,
        F0 = MatrixMap::fromFunction(0),
        F1 = MatrixMap::fromFunction(1),
        F2 = MatrixMap::fromFunction(2),
        F3 = MatrixMap::fromFunction(3),
        F4 = MatrixMap::fromFunction(4),
        Encoder = 40,
    };

    Key(int code, const KeyState &state) : _code(code), _state(state) {}

    int code() const { return _code; }

    const KeyState &state() const { return _state; }
    bool state(int code) const { return _state[code]; }

    bool shiftModifier() const { return _state[Shift]; }
    bool pageModifier() const { return _state[Page]; }

    bool is(int code) const { return _code == code; }

    bool isTrack() const { return MatrixMap::isTrack(_code); }
    int track() const { return MatrixMap::toTrack(_code); }

    bool isStep() const { return MatrixMap::isStep(_code); }
    int step() const { return MatrixMap::toStep(_code); }

    bool isFunction() const { return MatrixMap::isFunction(_code); }
    int function() const { return MatrixMap::toFunction(_code); }

    bool isPage() const { return _code == Page; };

    bool isTrackSelect() const { return !pageModifier() && isTrack(); }
    int trackSelect() const { return track(); }

    bool isPageSelect() const { return pageModifier() && (isTrack() || isStep()); }
    int pageSelect() const {
        if (isTrack()) {
            return track();
        } else if (isStep()) {
            return step() + 8;
        }
        return -1;
    }

    bool isContextMenu() const { return (is(Page) && shiftModifier()) || (is(Shift) && pageModifier()); }

    bool isEncoder() const { return is(Encoder); }

    bool isGlobal() const { return is(Start) || is(BPM); }

private:
    int _code;
    const KeyState &_state;
};
