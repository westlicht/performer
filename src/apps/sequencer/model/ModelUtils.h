#pragma once

#include "core/utils/StringBuilder.h"

#include <array>
#include <algorithm>

namespace ModelUtils {

static int adjusted(int value, int offset, int min, int max) {
    return std::max(min, std::min(max, value + offset));
}

template<typename Enum>
static Enum adjustedEnum(Enum value, int offset) {
    return Enum(adjusted(int(value), offset, 0, int(Enum::Last) - 1));
}

int adjustedByPowerOfTwo(int value, int offset, bool shift);

template<typename Step, size_t N>
static void shiftSteps(std::array<Step, N> &steps, int direction) {
    if (direction == 1) {
        for (int i = int(steps.size()) - 2; i >= 0; --i) {
            std::swap(steps[i], steps[i + 1]);
        }
    } else if (direction == -1) {
        for (int i = 0; i < int(steps.size()) - 1; ++i) {
            std::swap(steps[i], steps[i + 1]);
        }
    }
}

void printYesNo(StringBuilder &str, bool value);

};
