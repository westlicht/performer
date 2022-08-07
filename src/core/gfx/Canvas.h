#pragma once

#include "FrameBuffer.h"

#include <algorithm>

#include <cmath>
#include <cstdint>

enum class BlendMode {
    Set,
    Add,
    Sub,
};

enum Color {
    None = 0,
    Low = 0x3,
    MediumLow = 0x5,
    Medium = 0x7,
    MediumBright = 0xa,
    Bright = 0xf
};

enum class Font {
    Tiny,
    Small,
    Default = Tiny,
};

enum class HorizontalAlign {
    Left,
    Right,
    Center,
};

enum class VerticalAlign {
    Top,
    Bottom,
    Center,
};

class Canvas {
public:
    Canvas(FrameBuffer8bit &frameBuffer, float &brightness) :
        _frameBuffer(frameBuffer),
        _right(frameBuffer.width() - 1),
        _bottom(frameBuffer.height() - 1),
        _brightness(brightness)
    {
    }

    uint8_t color() const { return _color; }
    void setColorValue(uint8_t color) { _color = color * _brightness; }
    void setColor(Color color) { setColorValue(color); }

    BlendMode blendMode() const { return _blendMode; }
    void setBlendMode(BlendMode blendMode) { _blendMode = blendMode; }

    Font font() const { return _font; }
    void setFont(Font font) { _font = font; }

    void fill();
    void screensaver();

    void point(int x, int y);

    void hline(int x, int y, int w);
    void vline(int x, int y, int h);

    void line(float x0, float y0, float x1, float y1);

    void drawRect(int x, int y, int w, int h);
    void fillRect(int x, int y, int w, int h);

    void drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap);
    void drawBitmap4bit(int x, int y, int w, int h, const uint8_t *bitmap);

    void drawText(int x, int y, const char *str);
    void drawTextCentered(int x, int y, int w, int h, const char *str);
    void drawTextAligned(int x, int y, int w, int h, HorizontalAlign horizontalAlign, VerticalAlign verticalAlign, const char *str);
    void drawTextMultiline(int x, int y, int w, const char *str);

    int textWidth(const char *str);
    int textHeight(const char *str);


private:
    void hclip(int &x) {
        x = std::max(0, std::min(_right, x));
    }

    void vclip(int &y) {
        y = std::max(0, std::min(_bottom, y));
    }

    void clip(int &x, int &y) {
        hclip(x);
        vclip(y);
    }

    bool hinside(int x) {
        return x >= 0 && x <= _right;
    }

    bool vinside(int y) {
        return y >= 0 && y <= _bottom;
    }

    bool inside(int x, int y) {
        return hinside(x) && vinside(y);
    }

    template<typename Blit>
    void point(int x, int y) {
        Blit blit;
        if (inside(x, y)) {
            blit(_frameBuffer, x, y, _color);
        }
    }

    template<typename Blit>
    void hline(int x, int y, int w) {
        Blit blit;
        if (vinside(y)) {
            int x0 = x, x1 = x + w - 1;
            hclip(x0);
            hclip(x1);
            for (int x = x0; x <= x1; ++x) {
                blit(_frameBuffer, x, y, _color);
            }
        }
    }

    template<typename Blit>
    void vline(int x, int y, int h) {
        Blit blit;
        if (hinside(x)) {
            int y0 = y, y1 = y + h - 1;
            vclip(y0);
            vclip(y1);
            for (int y = y0; y <= y1; ++y) {
                blit(_frameBuffer, x, y, _color);
            }
        }
    }

    template<typename Blit>
    void line(float x0, float y0, float x1, float y1) {
        Blit blit;

        auto plot = [&] (int x, int y, float c) {
            if (inside(x, y)) {
                blit(_frameBuffer, x, y, _color * c);
            }
        };

        auto ipart = [] (float x) { return std::floor(x); };
        auto round = [] (float x) { return std::floor(x + 0.5f); };
        auto fpart = [] (float x) { return x - std::floor(x); };
        auto rfpart = [] (float x) { return 1.f - (x - std::floor(x)); };

        bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

        if (steep) {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        float dx = x1 - x0;
        float dy = y1 - y0;
        float gradient = dx == 0.f ? 1.f : dy / dx;

        // first endpoint
        int xend = round(x0);
        float yend = y0 + gradient * (xend - x0);
        float xgap = rfpart(x0 + 0.5f);
        int xpxl1 = xend;
        int ypxl1 = ipart(yend);
        if (steep) {
            plot(ypxl1,     xpxl1, rfpart(yend) * xgap);
            plot(ypxl1 + 1, xpxl1,  fpart(yend) * xgap);
        } else {
            plot(xpxl1, ypxl1,     rfpart(yend) * xgap);
            plot(xpxl1, ypxl1 + 1,  fpart(yend) * xgap);
        }
        float intery = yend + gradient;

        // second endpoint
        xend = round(x1);
        yend = y1 + gradient * (xend - x1);
        xgap = fpart(x1 + 0.5f);
        int xpxl2 = xend;
        int ypxl2 = ipart(yend);
        if (steep) {
            plot(ypxl2,     xpxl2, rfpart(yend) * xgap);
            plot(ypxl2 + 1, xpxl2,  fpart(yend) * xgap);
        } else {
            plot(xpxl2, ypxl2,    rfpart(yend) * xgap);
            plot(xpxl2, ypxl2 + 1, fpart(yend) * xgap);
        }

        // main loop
        if (steep) {
            for (int x = xpxl1 + 1; x < xpxl2; ++x) {
                plot(ipart(intery),     x, rfpart(intery));
                plot(ipart(intery) + 1, x,  fpart(intery));
                intery = intery + gradient;
            }
        } else {
            for (int x = xpxl1 + 1; x < xpxl2; ++x) {
                plot(x, ipart(intery),    rfpart(intery));
                plot(x, ipart(intery) + 1, fpart(intery));
                intery = intery + gradient;
            }
        }
    }

    template<typename Blit>
    void drawRect(int x, int y, int w, int h) {
        hline(x, y, w);
        hline(x, y + h - 1, w);
        vline(x, y + 1, h - 2);
        vline(x + w - 1, y + 1, h - 2);
    }

    template<typename Blit>
    void fillRect(int x, int y, int w, int h) {
        Blit blit;
        int x0 = x, x1 = x + w - 1;
        int y0 = y, y1 = y + h - 1;
        clip(x0, y0);
        clip(x1, y1);
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                blit(_frameBuffer, x, y, _color);
            }
        }
    }

    template<typename Blit, size_t Bpp>
    void drawBitmap(int x, int y, int w, int h, const uint8_t *bitmap) {
        Blit blit;
        int x0 = x, x1 = x + w - 1;
        int y0 = y, y1 = y + h - 1;
        if (x0 > _right || x1 < 0 || y0 > _bottom || y1 < 0) {
            return;
        }

        const uint8_t mask = (1 << Bpp) - 1;
        int shift = 0;
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                // uint8_t pixel = ((*bitmap >> shift) & mask) << (8 - Bpp);
                uint8_t pixel = ((*bitmap >> shift) & mask) * _color;
                shift += Bpp;
                if (shift >= 8) {
                    ++bitmap;
                    shift = 0;
                }
                if (inside(x, y)) {
                    blit(_frameBuffer, x, y, pixel);
                }
            }
        }
    }

    FrameBuffer8bit &_frameBuffer;
    int _right;
    int _bottom;
    uint8_t _color = 0xf;
    BlendMode _blendMode = BlendMode::Set;
    Font _font = Font::Default;
    float &_brightness;
};
