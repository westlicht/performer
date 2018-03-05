#include "Math.h"

uint32_t nextPowerOfTwo(uint32_t value, bool strict) {
    if (value == 0) {
        return 1;
    }
    if (!strict) {
        --value;
    }
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    return value + 1;
}

uint32_t prevPowerOfTwo(uint32_t value, bool strict) {
    if (value == 0) {
        return 0;
    }
    if (strict) {
        --value;
    }
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    return value - (value >> 1);
}
