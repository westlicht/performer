#pragma once

#include <cstdint>

class Types {
public:

    enum class TrackMode : uint8_t {
        Note,
        Curve,
        Last,
        Default = Note
    };

    static const char *trackModeName(TrackMode trackMode) {
        switch (trackMode) {
        case TrackMode::Note:    return "Note";
        case TrackMode::Curve:   return "Curve";
        case TrackMode::Last:    break;
        }
        return nullptr;
    }

}; // namespace Types
