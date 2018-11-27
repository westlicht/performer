#pragma once

#include "sim/Simulator.h"
#include "sim/FrameBuffer.h"

#include "SystemConfig.h"

#include <cstdint>
#include <cstring>

class Lcd {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    Lcd() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    void draw(uint8_t *frameBuffer) {
        std::memcpy(_frameBuffer.data(), frameBuffer, _frameBuffer.size());
        _simulator.writeLcd(_frameBuffer);
    }

private:
    sim::Simulator &_simulator;
    sim::FrameBuffer _frameBuffer;
};
