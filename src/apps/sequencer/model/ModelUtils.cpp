#include "ModelUtils.h"

#include "KnownDivisor.h"

#include "core/math/Math.h"

namespace ModelUtils {

int adjustedByStep(int value, int offset, int step, bool shift) {
    if (shift) {
        return value + offset;
    } else {
        if (offset > 0) {
            return ((value + offset * step) / step) * step;
        } else if (offset < 0) {
            return ((value - 1) / step) * step;
        } else {
            return value;
        }
    }
}

int adjustedByPowerOfTwo(int value, int offset, bool shift) {
    if (shift) {
        return value + offset;
    } else {
        if (offset > 0) {
            return nextPowerOfTwo(value);
        } else if (offset < 0) {
            return prevPowerOfTwo(value);
        } else {
            return value;
        }
    }
}

int adjustedByDivisor(int value, int offset, bool shift) {
    if (shift) {
        return value + offset;
    } else {
        if (offset > 0) {
            for (int i = 0; i < numKnownDivisors; ++i) {
                const auto &knownDivisor = knownDivisors[i];
                if (knownDivisor.divisor > value) {
                    return knownDivisor.divisor;
                }
            }
        } else if (offset < 0) {
            for (int i = numKnownDivisors - 1; i >= 0; --i) {
                const auto &knownDivisor = knownDivisors[i];
                if (knownDivisor.divisor < value) {
                    return knownDivisor.divisor;
                }
            }
        }
        return value;
    }
}

void printYesNo(StringBuilder &str, bool value) {
    str(value ? "yes" : "no");
}

void printDivisor(StringBuilder &str, int value) {
    for (int i = 0; i < numKnownDivisors; ++i) {
        const auto &knownDivisor = knownDivisors[i];
        if (value == knownDivisor.divisor) {
            if (knownDivisor.denominator == 1) {
                str("%d %d%c", value, knownDivisor.numerator, knownDivisor.type);
            } else {
                str("%d %d/%d%c", value, knownDivisor.numerator, knownDivisor.denominator, knownDivisor.type);
            }
            return;
        }
    }
    str("%d", value);
}

int divisorToIndex(int divisor) {
    for (int i = 0; i < numKnownDivisors; ++i) {
        const auto &knownDivisor = knownDivisors[i];
        if (divisor == knownDivisor.divisor) {
            return knownDivisor.index;
        }
    }
    return -1;
}

int indexToDivisor(int index) {
    for (int i = 0; i < numKnownDivisors; ++i) {
        const auto &knownDivisor = knownDivisors[i];
        if (index == knownDivisor.index) {
            return knownDivisor.divisor;
        }
    }
    return -1;
}

} // namespace ModelUtils
