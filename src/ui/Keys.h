#pragma once

namespace KeyCode {

    static constexpr int Step(int step) {
        return step < 8 ? (8 + step) : (step - 8);
    }

    static constexpr int Track(int track) {
        return 16 + track;
    }

} // namespace KeyCode

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
        Stop = 25,
        Shift = 26,
    };

    Key(int code) : _code(code) {}

    int code() const { return _code; }

    bool is(int code) const { return _code == code; }

    bool isTrack() const { return _code >= Track0 && _code <= Track7; }
    int track() const { return _code - Track0; }

    bool isStep() const { return _code >= Step8 && _code <= Step7; }
    int step() const { return _code >= Step0 ? _code - Step0 : _code - Step8 + 8; }

private:
    int _code;
};
