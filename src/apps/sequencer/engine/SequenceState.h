#pragma once

#include "model/Types.h"
#include "model/TrackSetup.h"

#include "core/utils/Random.h"

#include <cstdint>

class SequenceState {
public:
    int step() const { return _step; }
    int direction() const { return _direction; }

    void reset();

    void advanceFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng);
    void advanceAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng);

private:
    int8_t _step;
    int8_t _direction;
};
