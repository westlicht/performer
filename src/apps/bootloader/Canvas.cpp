#include "Canvas.h"
#include "Lcd.h"
#include "Font.h"

#include <cstring>

uint8_t Canvas::_frameBuffer[Width * Height];
uint8_t Canvas::_color;

void Canvas::show() {
    Lcd::draw(_frameBuffer);
}

void Canvas::fill() {
    std::memset(_frameBuffer, _color, sizeof(_frameBuffer));
}

void Canvas::point(int x, int y) {
    if (inside(x, y)) {
        write(x, y, _color);
    }
}

void Canvas::hline(int x, int y, int w) {
    if (vinside(y)) {
        int x0 = x, x1 = x + w - 1;
        hclip(x0);
        hclip(x1);
        for (int x = x0; x <= x1; ++x) {
            write(x, y, _color);
        }
    }
}

void Canvas::vline(int x, int y, int h) {
    if (hinside(x)) {
        int y0 = y, y1 = y + h - 1;
        vclip(y0);
        vclip(y1);
        for (int y = y0; y <= y1; ++y) {
            write(x, y, _color);
        }
    }
}

void Canvas::drawRect(int x, int y, int w, int h) {
    hline(x, y, w);
    hline(x, y + h - 1, w);
    vline(x, y + 1, h - 2);
    vline(x + w - 1, y + 1, h - 2);
}

void Canvas::fillRect(int x, int y, int w, int h) {
    int x0 = x, x1 = x + w - 1;
    int y0 = y, y1 = y + h - 1;
    clip(x0, y0);
    clip(x1, y1);
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            write(x, y, _color);
        }
    }
}

void Canvas::drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap) {
    int x0 = x, x1 = x + w - 1;
    int y0 = y, y1 = y + h - 1;
    if (x0 >= Width || x1 < 0 || y0 >= Height || y1 < 0) {
        return;
    }

    int shift = 0;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            uint8_t pixel = ((*bitmap >> shift) & 1) * _color;
            shift += 1;
            if (shift >= 8) {
                ++bitmap;
                shift = 0;
            }
            if (inside(x, y)) {
                write(x, y, pixel);
            }
        }
    }
}

void Canvas::drawText(int x, int y, const char *str) {
    int ox = x;
    while (*str != '\0') {
        auto c = *str++;
        if (c == '\n') {
            x = ox;
            y += font.yAdvance;
            continue;
        }
        if (c < font.first || c > font.last) {
            continue;
        }
        const auto &g = font.glyphs[c - font.first];
        const uint8_t *bitmap = &font.bitmap[g.offset];
        drawBitmap1bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap);
        x += g.xAdvance;
    }
}

int Canvas::textWidth(const char *str) {
    int width = 0;

    while (*str != '\0') {
        auto c = *str++;
        if (c < font.first || c > font.last) {
            continue;
        }
        const auto &g = font.glyphs[c - font.first];
        width += g.xAdvance;
    }

    return width;
}
