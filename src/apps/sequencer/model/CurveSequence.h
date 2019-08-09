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

    typedef UnsignedValue<6> Shape;
    typedef UnsignedValue<4> ShapeVariationProbability;
    typedef UnsignedValue<8> Min;
    typedef UnsignedValue<8> Max;
    typedef UnsignedValue<4> Gate;
    typedef UnsignedValue<3> GateProbability;

    enum class Layer {
        Shape,
        ShapeVariation,
        ShapeVariationProbability,
        Min,
        Max,
        Gate,
        GateProbability,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Shape:                      return "SHAPE";
        case Layer::ShapeVariation:             return "SHAPE VAR";
        case Layer::ShapeVariationProbability:  return "SHAPE PROB";
        case Layer::Min:                        return "MIN";
        case Layer::Max:                        return "MAX";
        case Layer::Gate:                       return "GATE";
        case Layer::GateProbability:            return "GATE PROB";
        case Layer::Last:                       break;
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

        int shape() const { return _data0.shape; }
        void setShape(int shape) {
            _data0.shape = clamp(shape, 0, int(Curve::Last) - 1);
        }

        // shapeVariation

        int shapeVariation() const { return _data0.shapeVariation; }
        void setShapeVariation(int shapeVariation) {
            _data0.shapeVariation = clamp(shapeVariation, 0, int(Curve::Last) - 1);
        }

        // shapeVariationProbability

        int shapeVariationProbability() const { return _data0.shapeVariationProbability; }
        void setShapeVariationProbability(int shapeVariationProbability) {
            _data0.shapeVariationProbability = clamp(shapeVariationProbability, 0, 8);
        }

        // min

        int min() const { return _data0.min; }
        void setMin(int min) {
            _data0.min = Min::clamp(min);
            _data0.max = std::max(max(), this->min());
        }

        float minNormalized() const { return float(min()) / Min::Max; }
        void setMinNormalized(float min) {
            setMin(int(std::round(min * Min::Max)));
        }

        // max

        int max() const { return _data0.max; }
        void setMax(int max) {
            _data0.max = Max::clamp(max);
            _data0.min = std::min(min(), this->max());
        }

        float maxNormalized() const { return float(max()) / Max::Max; }
        void setMaxNormalized(float max) {
            setMax(int(std::round(max * Max::Max)));
        }

        // gate

        int gate() const { return _data1.gate; }
        void setGate(int gate) {
            _data1.gate = Gate::clamp(gate);
        }

        // gateProbability

        int gateProbability() const { return _data1.gateProbability; }
        void setGateProbability(int gateProbability) {
            _data1.gateProbability = GateProbability::clamp(gateProbability);
        }

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
            return _data0.raw == other._data0.raw;
        }

        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        union {
            uint32_t raw;
            BitField<uint32_t, 0, Shape::Bits> shape;
            BitField<uint32_t, 6, Shape::Bits> shapeVariation;
            BitField<uint32_t, 12, ShapeVariationProbability::Bits> shapeVariationProbability;
            BitField<uint32_t, 16, Min::Bits> min;
            BitField<uint32_t, 24, Max::Bits> max;
        } _data0;
        union {
            uint16_t raw;
            BitField<uint16_t, 0, Gate::Bits> gate;
            BitField<uint16_t, 4, GateProbability::Bits> gateProbability;
            // 9 bits left
        } _data1;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackIndex

    int trackIndex() const { return _trackIndex; }

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

    int divisor() const { return _divisor.get(isRouted(Routing::Target::Divisor)); }
    void setDivisor(int divisor, bool routed = false) {
        _divisor.set(ModelUtils::clampDivisor(divisor), routed);
    }

    int indexedDivisor() const { return ModelUtils::divisorToIndex(divisor()); }
    void setIndexedDivisor(int index) {
        int divisor = ModelUtils::indexToDivisor(index);
        if (divisor > 0) {
            setDivisor(divisor);
        }
    }

    void editDivisor(int value, bool shift) {
        if (!isRouted(Routing::Target::Divisor)) {
            setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
        }
    }

    void printDivisor(StringBuilder &str) const {
        printRouted(str, Routing::Target::Divisor);
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
            str("%d %s", resetMeasure(), resetMeasure() > 1 ? "bars" : "bar");
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
        printRouted(str, Routing::Target::RunMode);
        str(Types::runModeName(runMode()));
    }

    // firstStep

    int firstStep() const {
        return _firstStep.get(isRouted(Routing::Target::FirstStep));
    }

    void setFirstStep(int firstStep, bool routed = false) {
        _firstStep.set(clamp(firstStep, 0, lastStep()), routed);
    }

    void editFirstStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::FirstStep)) {
            setFirstStep(firstStep() + value);
        }
    }

    void printFirstStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::FirstStep);
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const {
        // make sure last step is always >= first step even if stored value is invalid (due to routing changes)
        return std::max(firstStep(), int(_lastStep.get(isRouted(Routing::Target::LastStep))));
    }

    void setLastStep(int lastStep, bool routed = false) {
        _lastStep.set(clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1), routed);
    }

    void editLastStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::LastStep)) {
            setLastStep(lastStep() + value);
        }
    }

    void printLastStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::LastStep);
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

    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
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
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    void offsetFirstAndLastStep(int value) {
        value = clamp(value, -firstStep(), CONFIG_STEP_COUNT - 1 - lastStep());
        if (value > 0) {
            editLastStep(value, false);
            editFirstStep(value, false);
        } else {
            editFirstStep(value, false);
            editLastStep(value, false);
        }
    }

    int8_t _trackIndex = -1;
    Types::VoltageRange _range;
    Routable<uint16_t> _divisor;
    uint8_t _resetMeasure;
    Routable<Types::RunMode> _runMode;
    Routable<uint8_t> _firstStep;
    Routable<uint8_t> _lastStep;

    StepArray _steps;

    friend class CurveTrack;
};
