#pragma once

#include "Config.h"

#include <array>
#include <cstdint>

class Sequence {
public:
    enum PlayMode {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
    };

    struct Params {
        uint8_t playMode = Forward;
        uint8_t firstStep = 0;
        uint8_t lastStep = STEP_COUNT - 1;
    };

    struct Step {
        uint8_t active = 0;
        uint8_t note = 0;

        void toggle() {
            active = active ? 0 : 1;
        }
    };

    typedef std::array<Step, STEP_COUNT> StepArray;

    void setPlayMode(PlayMode playMode) { _params.playMode = playMode; }
    PlayMode playMode() const { return PlayMode(_params.playMode); }

    void setFirstStep(uint8_t firstStep) { _params.firstStep = firstStep; }
    uint8_t firstStep() const { return _params.firstStep; }

    void setLastStep(uint8_t lastStep) { _params.lastStep = lastStep; }
    uint8_t lastStep() const { return _params.lastStep; }

    const Params &params() const { return _params; }
          Params &params()       { return _params; }

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

private:
    Params _params;
    StepArray _steps;
};
