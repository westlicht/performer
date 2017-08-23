#pragma once

#include "sim/Simulator.h"
#include "sim/widgets/Display.h"

#include "Config.h"

#include <cstdint>

class LCD {
public:
    LCD() :
        _simulator(sim::Simulator::instance())
    {
        sim::Vector2i resolution(CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT);
        _display = _simulator.window().createWidget<sim::Display>(
            sim::Vector2i(70, 10),
            resolution * 2 + sim::Vector2i(2, 2),
            resolution
        );
    }

    void draw(uint8_t *frameBuffer) {
        _display->draw(frameBuffer);
    }

private:
    sim::Simulator &_simulator;
    sim::Display::Ptr _display;
};
