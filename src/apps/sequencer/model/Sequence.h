#pragma once

#include "Config.h"
#include "Serialize.h"
#include "NoteSequence.h"
#include "CurveSequence.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class Sequence {
public:
    enum class PlayMode : uint8_t {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Forward:     return "Forward";
        case PlayMode::Backward:    return "Backward";
        case PlayMode::PingPong:    return "PingPong";
        case PlayMode::Pendulum:    return "Pendulum";
        case PlayMode::Random:      return "Random";
        case PlayMode::Last:        break;
        }
        return nullptr;
    }

    Sequence() {
        _sequence.noteSequence.setDefault();
    }

    // playMode

    PlayMode playMode() const { return _playMode; }
    void setPlayMode(PlayMode playMode) { _playMode = playMode; }

    // firstStep

    int firstStep() const { return _firstStep; }
    void setFirstStep(int firstStep) {
        _firstStep = uint8_t(std::max(0, std::min(firstStep, int(_lastStep))));
    }

    // lastStep

    int lastStep() const { return _lastStep; }
    void setLastStep(int lastStep) {
        _lastStep = uint8_t(std::min(CONFIG_STEP_COUNT - 1, std::max(lastStep, int(_firstStep))));
    }

    // sequence

    const NoteSequence &noteSequence() const { return _sequence.noteSequence; }
          NoteSequence &noteSequence()       { return _sequence.noteSequence; }

    const CurveSequence &curveSequence() const { return _sequence.curveSequence; }
          CurveSequence &curveSequence()       { return _sequence.curveSequence; }

    // Serialization

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    PlayMode _playMode = PlayMode::Forward;
    uint8_t _firstStep = 0;
    uint8_t _lastStep = CONFIG_STEP_COUNT - 1;

    union {
        NoteSequence noteSequence;
        CurveSequence curveSequence;
    } _sequence;
};
