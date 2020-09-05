#pragma once

#include "model/Types.h"

#include "core/utils/Random.h"

#include <cstdint>

class SequenceState {
public:
    int step() const { return _step; }
    int prevStep() const { return _prevStep; }
    int nextStep() const { return _nextStep; }
    int direction() const { return _direction; }
    uint32_t iteration() const { return _iteration; }


    void reset();

    void advanceAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng);
    void calculateNextStepAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng);

    void advanceFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng);
    void calculateNextStepFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng);

private:
    void advanceRandomWalk(int firstStep, int lastStep, Random &rng);

    int8_t _step;
    int8_t _prevStep;
    int8_t _nextStep = -1;
    int8_t _direction;
    uint32_t _iteration;
    uint32_t _nextIteration;
};
