#pragma once

#include <array>

class Sequence {
public:
    struct Step {
        static const int Num = 16;
        typedef std::array<Step, Num> Array;

        uint8_t active = 0;
        uint8_t note = 0;

        void toggle() {
            active = active ? 0 : 1;
        }
    };

    const Step::Array &steps() const { return _steps; }
          Step::Array &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }
    
private:
    Step::Array _steps;
};
