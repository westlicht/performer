#pragma once

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <algorithm>
#include <bitset>

namespace ModelUtils {

template<typename Enum>
static Enum clampedEnum(Enum value) {
    return Enum(clamp(int(value), 0, int(Enum::Last) - 1));
}

static int adjusted(int value, int offset, int min, int max) {
    return clamp(value + offset, min, max);
}

template<typename Enum>
static Enum adjustedEnum(Enum value, int offset) {
    return Enum(adjusted(int(value), offset, 0, int(Enum::Last) - 1));
}

int adjustedByStep(int value, int offset, int step, bool shift);
int adjustedByPowerOfTwo(int value, int offset, bool shift);
int adjustedByDivisor(int value, int offset, bool shift);

void printYesNo(StringBuilder &str, bool value);
void printDivisor(StringBuilder &str, int value);

int divisorToIndex(int divisor);
int indexToDivisor(int index);

static int clampDivisor(int divisor) {
    return clamp(divisor, 1, 768);
}

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

template<typename Step, size_t N>
static void duplicateSteps(std::array<Step, N> &steps, int firstStep, int lastStep) {
    for (int src = firstStep; src <= lastStep; ++src) {
        int dst = src + (lastStep - firstStep + 1);
        if (dst < int(steps.size())) {
            steps[dst] = steps[src];
        }
    }
}

template<typename Step, size_t N>
static void copySteps(
    const std::array<Step, N> &src, const std::bitset<N> &srcSelected,
    std::array<Step, N> &dst, const std::bitset<N> &dstSelected
) {
    auto nextSelected = [] (const std::bitset<N> &selected, int index) {
        if (selected.none()) {
            return (index + 1) % int(N);
        } else {
            do {
                index = (index + 1) % int(N);
            } while (!selected[index]);
            return index;
        }
    };

    int srcIndex = -1;

    for (size_t dstIndex = 0; dstIndex < N; ++dstIndex) {
        if (dstSelected.none() || dstSelected[dstIndex]) {
            srcIndex = nextSelected(srcSelected, srcIndex);
            dst[dstIndex] = src[srcIndex];
        }
    }
}

} // namespace ModelUtils
