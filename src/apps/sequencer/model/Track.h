#pragma once

#include "Serialize.h"

#include <cstdint>
#include <cstring>

class Track {
public:
    enum class Mode : uint8_t {
        Note,
        Curve,
        Last,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Note:    return "Note";
        case Mode::Curve:   return "Curve";
        case Mode::Last:    break;
        }
        return nullptr;
    }

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    // fillMode

    // Serialization

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

    bool operator==(const Track &other) const {
        return std::memcmp(this, &other, sizeof(Track)) == 0;
    }
    bool operator!=(const Track &other) const {
        return !(*this == other);
    }

private:
    Mode _mode = Mode::Note;
};
