#pragma once

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <algorithm>
#include <bitset>

namespace ModelUtils {

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
    if (srcSelected.none() || dstSelected.none()) {
        return;
    }

    size_t srcFirstIndex = 0;
    for (size_t srcIndex = 0; srcIndex < dst.size(); ++srcIndex) {
        if (srcSelected[srcIndex]) {
            srcFirstIndex = srcIndex;
            break;
        }
    }

    bool copy = false;
    for (size_t dstIndex = 0; dstIndex < dst.size(); ++dstIndex) {
        size_t srcIndex;
        if (dstSelected[dstIndex]) {
            copy = true;
            srcIndex = srcFirstIndex;
        }
        if (copy) {
            dst[dstIndex] = src[srcIndex];
            ++srcIndex;
            if (srcIndex >= N) {
                break;
            }
        }
    }
}

} // namespace ModelUtils
