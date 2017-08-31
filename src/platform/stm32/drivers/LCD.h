#pragma once

#include <cstdint>

class LCD {
public:
    void init();

    void draw(uint8_t *frameBuffer);

private:
    static const int Rows = 64;
    static const int Cols = 256;

    void sendCmd(uint8_t cmd);
    void sendData(uint8_t data);

    void setColAddr(uint8_t a, uint8_t b);
    void setRowAddr(uint8_t a, uint8_t b);
    void setWrite();

    uint32_t _frameBuffer[Rows * Cols / 8];
};
