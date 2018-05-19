#pragma once

#include "Config.h"

#include <cstdint>
#include <cstdlib>

class Lcd {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    static void init();
    static void deinit();

    static void draw(uint8_t *frameBuffer);

private:
    static void sendCmd(uint8_t cmd);
    static void sendData(uint8_t data);

    static void initialize();

    static void setColAddr(uint8_t a, uint8_t b);
    static void setRowAddr(uint8_t a, uint8_t b);
    static void setWrite();
};
