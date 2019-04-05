#pragma once

#include "core/Debug.h"

namespace SequenceUtils {

static int rotateStep(int step, int firstStep, int lastStep, int rotate) {
    ASSERT(firstStep <= lastStep, "invalid first/last step");

    int stepCount = lastStep - firstStep + 1;
    step = firstStep + (step - firstStep + rotate) % stepCount;
    return step + (step < 0 ? stepCount : 0);
}

} // namespace SequenceUtils
