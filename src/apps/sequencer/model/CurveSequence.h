#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"

#include "engine/Curve.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class CurveSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<4> Shape;
    typedef UnsignedValue<8> Min;
    typedef UnsignedValue<8> Max;

    class Step {
    public:
        // shape

        int shape() const { return _shape; }
        void setShape(int shape) { _shape = clamp(shape, 0, int(Curve::Last)); }

        // min

        int min() const { return _min; }
        void setMin(int min) { _min = min; }
        float minNormalized() const { return float(_min) / CurveSequence::Min::Max; }

        // max

        int max() const { return _max; }
        void setMax(int max) { _max = max; }
        float maxNormalized() const { return float(_max) / CurveSequence::Max::Max; }


        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        uint8_t _shape;
        uint8_t _min;
        uint8_t _max;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    enum class Range : uint8_t {
        Unipolar1V,
        Unipolar2V,
        Unipolar3V,
        Unipolar4V,
        Unipolar5V,
        Bipolar1V,
        Bipolar2V,
        Bipolar3V,
        Bipolar4V,
        Bipolar5V,
        Last
    };

    static const char *rangeName(Range range) {
        switch (range) {
        case Range::Unipolar1V: return "1V Unipolar";
        case Range::Unipolar2V: return "2V Unipolar";
        case Range::Unipolar3V: return "3V Unipolar";
        case Range::Unipolar4V: return "4V Unipolar";
        case Range::Unipolar5V: return "5V Unipolar";
        case Range::Bipolar1V:  return "1V Bipolar";
        case Range::Bipolar2V:  return "2V Bipolar";
        case Range::Bipolar3V:  return "3V Bipolar";
        case Range::Bipolar4V:  return "4V Bipolar";
        case Range::Bipolar5V:  return "5V Bipolar";
        case Range::Last:       break;
        }
        return nullptr;
    }

    static std::array<float, 2> rangeValues(Range range) {
        switch (range) {
        case Range::Unipolar1V: return {{ 0.f, 1.f }};
        case Range::Unipolar2V: return {{ 0.f, 2.f }};
        case Range::Unipolar3V: return {{ 0.f, 3.f }};
        case Range::Unipolar4V: return {{ 0.f, 4.f }};
        case Range::Unipolar5V: return {{ 0.f, 5.f }};
        case Range::Bipolar1V:  return {{ -1.f, 1.f }};
        case Range::Bipolar2V:  return {{ -2.f, 2.f }};
        case Range::Bipolar3V:  return {{ -3.f, 3.f }};
        case Range::Bipolar4V:  return {{ -4.f, 4.f }};
        case Range::Bipolar5V:  return {{ -5.f, 5.f }};
        case Range::Last:       break;
        }
        return {{ 0.f, 0.f }};
    }

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

    //----------------------------------------
    // Properties
    //----------------------------------------

    // range

    Range range() const { return _range; }
    void setRange(Range range) {
        _range = range;
    }

    void editRange(int value, bool shift) {
        setRange(ModelUtils::adjustedEnum(range(), value));
    }

    void printRange(StringBuilder &str) const {
        str(rangeName(range()));
    }

    // divisor

    int divisor() const { return _divisor; }
    void setDivisor(int divisor) {
        _divisor = clamp(divisor, 1, 192);
    }

    void editDivisor(int value, bool shift) {
        setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
    }

    void printDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, divisor());
    }

    // resetMeasure

    int resetMeasure() const { return _resetMeasure; }
    void setResetMeasure(int resetMeasure) {
        _resetMeasure = clamp(resetMeasure, 0, 128);
    }

    void editResetMeasure(int value, bool shift) {
        setResetMeasure(ModelUtils::adjustedByPowerOfTwo(resetMeasure(), value, shift));
    }

    void printResetMeasure(StringBuilder &str) const {
        if (resetMeasure() == 0) {
            str("off");
        } else {
            str("%d", resetMeasure());
        }
    }

    // playMode

    PlayMode playMode() const { return _playMode; }
    void setPlayMode(PlayMode playMode) {
        _playMode = playMode;
    }

    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    void printPlayMode(StringBuilder &str) const {
        str(playModeName(playMode()));
    }

    // firstStep

    int firstStep() const { return _firstStep; }
    void setFirstStep(int firstStep) {
        _firstStep = clamp(firstStep, 0, lastStep());
    }

    void editFirstStep(int value, bool shift) {
        setFirstStep(firstStep() + value);
    }

    void printFirstStep(StringBuilder &str) const {
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const { return _lastStep; }
    void setLastStep(int lastStep) {
        _lastStep = clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1);
    }

    void editLastStep(int value, bool shift) {
        setLastStep(lastStep() + value);
    }

    void printLastStep(StringBuilder &str) const {
        str("%d", lastStep() + 1);
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void setShapes(std::initializer_list<int> shapes);

    void shift(int direction);

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Range _range;
    uint8_t _divisor;
    uint8_t _resetMeasure;
    PlayMode _playMode;
    uint8_t _firstStep;
    uint8_t _lastStep;
    StepArray _steps;
};
