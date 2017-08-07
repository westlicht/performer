#pragma once

#include "Config.h"

#include <array>
#include <cstdint>
#include <initializer_list>

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
        uint8_t lastStep = CONFIG_STEP_COUNT - 1;
    };

    struct Step {
        uint8_t active = 0;
        uint8_t note = 0;

        void toggle() {
            active = active ? 0 : 1;
        }
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

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


    // utility functions
    void setGates(std::initializer_list<int> gates) {
        size_t step = 0;
        for (auto gate : gates) {
            if (step < _steps.size()) {
                _steps[step++].active = gate;
            }
        }
    }
    void setNotes(std::initializer_list<int> notes) {
        size_t step = 0;
        for (auto note : notes) {
            if (step < _steps.size()) {
                _steps[step++].note = note;
            }
        }
    }

private:
    Params _params;
    StepArray _steps;
};
