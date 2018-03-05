#include "ModelUtils.h"

#include "core/math/Math.h"

namespace ModelUtils {

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
