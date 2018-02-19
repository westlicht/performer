#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"

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
        void setShape(int shape) { _shape = shape; }

        // min

        int min() const { return _min; }
        void setMin(int min) { _min = min; }

        // max

        int max() const { return _max; }
        void setMax(int max) { _max = max; }


        void clear();

        void write(WriteContext &context, int index) const;
        void read(ReadContext &context, int index);

    private:
        uint8_t _shape;
        uint8_t _min;
        uint8_t _max;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

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

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    StepArray _steps;
};
