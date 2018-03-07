#include "ModelUtils.h"

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

void printYesNo(StringBuilder &str, bool value) {
    str(value ? "yes" : "no");
}

} // namespace ModelUtils
