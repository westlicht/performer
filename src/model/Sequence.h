#pragma once

#include "Config.h"
#include "Serialize.h"

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

    struct Step {
        uint8_t active = 0;
        uint8_t note = 0;

        void toggle() {
            active = active ? 0 : 1;
        }

        // Serialization

        void write(ModelWriter &writer) const {
            writer.write(active);
            writer.write(note);
        }

        void read(ModelReader &reader) {
            reader.read(active);
            reader.read(note);
        }

    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    void setPlayMode(PlayMode playMode) {
        _playMode = PlayMode(std::max(0, std::min(int(Random), int(playMode))));
    }
    PlayMode playMode() const { return PlayMode(_playMode); }

    void setFirstStep(int firstStep) {
        _firstStep = uint8_t(std::max(0, std::min(firstStep, int(_lastStep))));
    }
    int firstStep() const { return _firstStep; }

    void setLastStep(int lastStep) {
        _lastStep = uint8_t(std::min(CONFIG_STEP_COUNT - 1, std::max(lastStep, int(_firstStep))));
    }
    int lastStep() const { return _lastStep; }

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

    // Serialization

    void write(ModelWriter &writer) const {
        writer.write(_playMode);
        writer.write(_firstStep);
        writer.write(_lastStep);
        for (const auto &step : _steps) {
            step.write(writer);
        }
    }

    void read(ModelReader &reader) {
        reader.read(_playMode);
        reader.read(_firstStep);
        reader.read(_lastStep);
        for (auto &step : _steps) {
            step.read(reader);
        }
    }

private:
    uint8_t _playMode = Forward;
    uint8_t _firstStep = 0;
    uint8_t _lastStep = CONFIG_STEP_COUNT - 1;
    StepArray _steps;
};
