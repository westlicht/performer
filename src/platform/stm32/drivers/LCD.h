#pragma once

#include "SystemConfig.h"

#include <cstdint>
#include <cstdlib>

class LCD {
public:
    static constexpr size_t Width = CONFIG_LCD_WIDTH;
    static constexpr size_t Height = CONFIG_LCD_HEIGHT;

    void init();

    void draw(uint8_t *frameBuffer);

private:
    void sendCmd(uint8_t cmd);
    void sendData(uint8_t data);

    void initialize();

    void setColAddr(uint8_t a, uint8_t b);
    void setRowAddr(uint8_t a, uint8_t b);
    void setWrite();

    uint32_t _frameBuffer[Width * Height / 8];
};
