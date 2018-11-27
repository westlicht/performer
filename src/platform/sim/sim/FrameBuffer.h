#pragma once

#include "TargetConfig.h"

#include <array>

#include <cstdint>

namespace sim {

typedef std::array<uint8_t, TargetConfig::LcdWidth * TargetConfig::LcdHeight> FrameBuffer;

} // namespace sim
