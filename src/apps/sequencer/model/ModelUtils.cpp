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
                str("%d 1%c", value, knownDivisor.type);
            } else {
                str("%d 1/%d%c", value, knownDivisor.denominator, knownDivisor.type);
            }
            return;
        }
    }
    str("%d", value);
}

} // namespace ModelUtils
