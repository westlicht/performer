#pragma once

#include "Serialize.h"

#include <cstdint>

class Track {
public:
    enum class Mode : uint8_t {
        Note,
        Curve,
    };

    // mode

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    // Serialization

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    Mode _mode = Mode::Note;
};
