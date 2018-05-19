#pragma once

#include "Config.h"

#include <algorithm>

#include <cstdint>

class Canvas {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    static void show();

    static uint8_t color() { return _color; }
    static void setColor(uint8_t color) { _color = color; }

    static void fill();

    static void point(int x, int y);

    static void hline(int x, int y, int w);
    static void vline(int x, int y, int h);

    static void drawRect(int x, int y, int w, int h);
    static void fillRect(int x, int y, int w, int h);

    static void drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap);
    static void drawText(int x, int y, const char *str);
    static int textWidth(const char *str);

private:
    static void hclip(int &x) {
        x = std::max(0, std::min(Width - 1, x));
    }

    static void vclip(int &y) {
        y = std::max(0, std::min(Width - 1, y));
    }

    static void clip(int &x, int &y) {
        hclip(x);
        vclip(y);
    }

    static bool hinside(int x) {
        return x >= 0 && x <= Width - 1;
    }

    static bool vinside(int y) {
        return y >= 0 && y <= Height - 1;
    }

    static bool inside(int x, int y) {
        return hinside(x) && vinside(y);
    }

    static void write(int x, int y, uint8_t c) {
        _frameBuffer[y * Width + x] = c;
    }

    static uint8_t _frameBuffer[Width * Height];
    static uint8_t _color;
};
