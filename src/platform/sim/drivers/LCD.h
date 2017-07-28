#pragma once

#include "sim/Simulator.h"
#include "sim/widgets/Display.h"

#include "Config.h"

#include <cstdint>

class LCD {
public:
    LCD(sim::Simulator &simulator) :
        _simulator(simulator)
    {
        sim::Vector2i resolution(LCD_WIDTH, LCD_HEIGHT);
        _display = std::make_shared<sim::Display>(sim::Vector2i(10, 10), resolution * 2 + sim::Vector2i(2, 2), resolution);
        _simulator.window().addWidget(_display);
    }

    void draw(uint8_t *frameBuffer) {
        _display->draw(frameBuffer);
    }

private:
    sim::Simulator &_simulator;
    std::shared_ptr<sim::Display> _display;
};
