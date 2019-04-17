#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "Curve.h"
#include "Routing.h"

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

    typedef UnsignedValue<8> Min;
    typedef UnsignedValue<8> Max;

    enum class Layer {
        Shape,
        Min,
        Max,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Shape:  return "SHAPE";
        case Layer::Min:    return "MIN";
        case Layer::Max:    return "MAX";
        case Layer::Last:   break;
        }
        return nullptr;
    }

    static Types::LayerRange layerRange(Layer layer);

    class Step {
    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // shape

        int shape() const { return _shape; }
        void setShape(int shape) {
            _shape = clamp(shape, 0, int(Curve::Last) - 1);
        }

        // min

        int min() const { return _min; }
        void setMin(int min) {
            _min = Min::clamp(min);
            _max = std::max(_max, _min);
        }

        float minNormalized() const { return float(_min) / Min::Max; }

        // max

        int max() const { return _max; }
        void setMax(int max) {
            _max = Max::clamp(max);
            _min = std::min(_min, _max);
        }

        float maxNormalized() const { return float(_max) / Max::Max; }

        int layerValue(Layer layer) const;
        void setLayerValue(Layer layer, int value);

        //----------------------------------------
        // Methods
        //----------------------------------------

        Step() { clear(); }

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

        bool operator==(const Step &other) const {
            return _shape == other._shape && _min == other._min && _max == other._max;
        }

        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        uint8_t _shape;
        uint8_t _min;
        uint8_t _max;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // range

    Types::VoltageRange range() const { return _range; }
    void setRange(Types::VoltageRange range) {
        _range = ModelUtils::clampedEnum(range);
    }

    void editRange(int value, bool shift) {
        setRange(ModelUtils::adjustedEnum(range(), value));
    }

    void printRange(StringBuilder &str) const {
        str(Types::voltageRangeName(range()));
    }

    // divisor

    int divisor() const { return _divisor; }
    void setDivisor(int divisor) {
        _divisor = clamp(divisor, 1, 192);
    }

    int indexedDivisor() const { return ModelUtils::divisorToIndex(divisor()); }
    void setIndexedDivisor(int index) {
        int divisor = ModelUtils::indexToDivisor(index);
        if (divisor > 0) {
            setDivisor(divisor);
        }
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

    // runMode

    Types::RunMode runMode() const { return _runMode.get(isRouted(Routing::Target::RunMode)); }
    void setRunMode(Types::RunMode runMode, bool routed = false) {
        _runMode.set(ModelUtils::clampedEnum(runMode), routed);
    }

    void editRunMode(int value, bool shift) {
        if (!isRouted(Routing::Target::RunMode)) {
            setRunMode(ModelUtils::adjustedEnum(runMode(), value));
        }
    }

    void printRunMode(StringBuilder &str) const {
        _routed.print(str, Routing::Target::RunMode);
        str(Types::runModeName(runMode()));
    }

    // firstStep

    int firstStep() const { return _firstStep.get(isRouted(Routing::Target::FirstStep)); }
    void setFirstStep(int firstStep, bool routed = false) {
        _firstStep.set(clamp(firstStep, 0, lastStep()), routed);
    }

    void editFirstStep(int value, bool shift) {
        if (!isRouted(Routing::Target::FirstStep)) {
            setFirstStep(firstStep() + value);
        }
    }

    void printFirstStep(StringBuilder &str) const {
        _routed.print(str, Routing::Target::FirstStep);
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const { return _lastStep.get(isRouted(Routing::Target::LastStep)); }
    void setLastStep(int lastStep, bool routed = false) {
        _lastStep.set(clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1), routed);
    }

    void editLastStep(int value, bool shift) {
        if (!isRouted(Routing::Target::LastStep)) {
            setLastStep(lastStep() + value);
        }
    }

    void printLastStep(StringBuilder &str) const {
        _routed.print(str, Routing::Target::LastStep);
        str("%d", lastStep() + 1);
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return _routed.has(target); }
    inline void setRouted(Routing::Target target, bool routed) { _routed.set(target, routed); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------

    CurveSequence() { clear(); }

    void clear();
    void clearSteps();

    bool isEdited() const;

    void setShapes(std::initializer_list<int> shapes);

    void shiftSteps(int direction);

    void duplicateSteps();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::VoltageRange _range;
    uint8_t _divisor;
    uint8_t _resetMeasure;
    Routable<Types::RunMode> _runMode;
    Routable<uint8_t> _firstStep;
    Routable<uint8_t> _lastStep;

    RoutableSet<Routing::Target::SequenceFirst, Routing::Target::SequenceLast> _routed;

    StepArray _steps;
};
