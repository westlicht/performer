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

    enum class PlayMode : uint8_t {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Forward:     return "Forward";
        case PlayMode::Backward:    return "Backward";
        case PlayMode::PingPong:    return "PingPong";
        case PlayMode::Pendulum:    return "Pendulum";
        case PlayMode::Random:      return "Random";
        case PlayMode::Last:        break;
        }
        return nullptr;
    }


}; // namespace Types
