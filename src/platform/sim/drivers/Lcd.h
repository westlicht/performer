#pragma once

#include "sim/Simulator.h"

#include "SystemConfig.h"

#include <cstdint>

class Lcd {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    Lcd() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    void draw(uint8_t *frameBuffer) {
        _simulator.writeLcd(frameBuffer);
    }

private:
    sim::Simulator &_simulator;
};
