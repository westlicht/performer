#pragma once

static int adjusted(int value, int offset, int min, int max) {
    return std::max(min, std::min(max, value + offset));
}

template<typename Enum>
static Enum adjustedEnum(Enum value, int offset) {
    return Enum(adjusted(int(value), offset, 0, int(Enum::Last) - 1));
}
