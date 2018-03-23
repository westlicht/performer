#pragma once

#include "sim/Simulator.h"
#include "sim/widgets/Display.h"

#include "SystemConfig.h"

#include <string>

#include <cstdint>

class Lcd {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    Lcd() :
        _simulator(sim::Simulator::instance())
    {
        sim::Vector2i resolution(CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT);
        _display = _simulator.window().createWidget<sim::Display>(
            sim::Vector2i(64 + 16, 8),
            resolution * 2 + sim::Vector2i(2, 2),
            resolution
        );

        _simulator.setScreenshotCallback([&] (const std::string &filename) {
            _display->screenshot(filename);
        });
    }

    void init() {}

    void draw(uint8_t *frameBuffer) {
        _display->draw(frameBuffer);
    }

private:
    sim::Simulator &_simulator;
    sim::Display::Ptr _display;
};
