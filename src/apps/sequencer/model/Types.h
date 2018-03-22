#pragma once

#include <cstdint>

class Types {
public:
    enum class RunMode : uint8_t {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
        Last
    };

    static const char *runModeName(RunMode runMode) {
        switch (runMode) {
        case RunMode::Forward:  return "Forward";
        case RunMode::Backward: return "Backward";
        case RunMode::PingPong: return "PingPong";
        case RunMode::Pendulum: return "Pendulum";
        case RunMode::Random:   return "Random";
        case RunMode::Last:     break;
        }
        return nullptr;
    }

}; // namespace Types
