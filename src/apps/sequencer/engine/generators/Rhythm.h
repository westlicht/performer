#pragma once

#include "Config.h"

#include "RhythmString.h"

namespace Rhythm {

    typedef RhythmString<CONFIG_STEP_COUNT> Pattern;

    Pattern euclidean(int beats, int steps);

} // namespace Rhythm
