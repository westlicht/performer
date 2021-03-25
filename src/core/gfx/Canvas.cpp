#include "Canvas.h"
#include "Blit.h"

#include <stdio.h>

// Font definitions
#include "fonts/tiny5x5.h"
#include "fonts/ati8x8.h"

static const BitmapFont &bitmapFont(Font font) {
    switch (font) {
    case Font::Tiny: return tiny5x5;
    case Font::Small: return ati8x8;
    default: return tiny5x5;
    }
}

static const int bitmapFontHeight(Font font) {
    switch (font) {
    case Font::Tiny: return 6;
    case Font::Small: return 10;
    default: return 0;
    }
}

static const int bitmapFontOffset(Font font) {
    switch (font) {
    case Font::Tiny: return 5;
    case Font::Small: return 8;
    default: return 0;
    }
}


void Canvas::fill() {
    _frameBuffer.fill(_color);
}

void Canvas::screensaver() {
    _frameBuffer.fill(0x0);
}

void Canvas::point(int x, int y) {
    switch (_blendMode) {
    case BlendMode::Set: point<blit::set>(x, y); break;
    case BlendMode::Add: point<blit::add>(x, y); break;
    case BlendMode::Sub: point<blit::sub>(x, y); break;
    }
}

void Canvas::hline(int x, int y, int w) {
    switch (_blendMode) {
    case BlendMode::Set: hline<blit::set>(x, y, w); break;
    case BlendMode::Add: hline<blit::add>(x, y, w); break;
    case BlendMode::Sub: hline<blit::sub>(x, y, w); break;
    }
}

void Canvas::vline(int x, int y, int h) {
    switch (_blendMode) {
    case BlendMode::Set: vline<blit::set>(x, y, h); break;
    case BlendMode::Add: vline<blit::add>(x, y, h); break;
    case BlendMode::Sub: vline<blit::sub>(x, y, h); break;
    }
}

void Canvas::line(float x0, float y0, float x1, float y1) {
    switch (_blendMode) {
    case BlendMode::Set: line<blit::set>(x0, y0, x1, y1); break;
    case BlendMode::Add: line<blit::add>(x0, y0, x1, y1); break;
    case BlendMode::Sub: line<blit::sub>(x0, y0, x1, y1); break;
    }
}

void Canvas::drawRect(int x, int y, int w, int h) {
    switch (_blendMode) {
    case BlendMode::Set: drawRect<blit::set>(x, y, w, h); break;
    case BlendMode::Add: drawRect<blit::add>(x, y, w, h); break;
    case BlendMode::Sub: drawRect<blit::sub>(x, y, w, h); break;
    }
}

void Canvas::fillRect(int x, int y, int w, int h) {
    switch (_blendMode) {
    case BlendMode::Set: fillRect<blit::set>(x, y, w, h); break;
    case BlendMode::Add: fillRect<blit::add>(x, y, w, h); break;
    case BlendMode::Sub: fillRect<blit::sub>(x, y, w, h); break;
    }
}

void Canvas::drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap) {
    switch (_blendMode) {
    case BlendMode::Set: drawBitmap<blit::set, 1>(x, y, w, h, bitmap); break;
    case BlendMode::Add: drawBitmap<blit::add, 1>(x, y, w, h, bitmap); break;
    case BlendMode::Sub: drawBitmap<blit::sub, 1>(x, y, w, h, bitmap); break;
    }
}

void Canvas::drawBitmap4bit(int x, int y, int w, int h, const uint8_t *bitmap) {
    switch (_blendMode) {
    case BlendMode::Set: drawBitmap<blit::set, 4>(x, y, w, h, bitmap); break;
    case BlendMode::Add: drawBitmap<blit::add, 4>(x, y, w, h, bitmap); break;
    case BlendMode::Sub: drawBitmap<blit::sub, 4>(x, y, w, h, bitmap); break;
    }
}

void Canvas::drawText(int x, int y, const char *str) {
    const auto &font = bitmapFont(_font);

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
        switch (font.bpp) {
        case 1: drawBitmap1bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap); break;
        case 4: drawBitmap4bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap); break;
        }
        x += g.xAdvance;
    }
}

void Canvas::drawTextCentered(int x, int y, int w, int h, const char *str) {
    drawTextAligned(x, y, w, h, HorizontalAlign::Center, VerticalAlign::Center, str);
}

void Canvas::drawTextAligned(int x, int y, int w, int h, HorizontalAlign horizontalAlign, VerticalAlign verticalAlign, const char *str) {
    // drawRect(x, y, w, h);

    switch (horizontalAlign) {
    case HorizontalAlign::Left:
        break;
    case HorizontalAlign::Right:
        x += w - textWidth(str);
        break;
    case HorizontalAlign::Center:
        x += (w - textWidth(str) + 1) / 2;
        break;
    }

    switch (verticalAlign) {
    case VerticalAlign::Top:
        break;
    case VerticalAlign::Bottom:
        y += h - textHeight(str);
        break;
    case VerticalAlign::Center:
        y += (h - textHeight(str) + 1) / 2;
        break;
    }
    y += bitmapFontOffset(_font);

    drawText(x, y, str);
}

void Canvas::drawTextMultiline(int x, int y, int w, const char *str) {
    const auto &font = bitmapFont(_font);

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
        if (x + g.xAdvance >= ox + w) {
            x = ox;
            y += font.yAdvance;
            str--;
            continue;
        }
        const uint8_t *bitmap = &font.bitmap[g.offset];
        switch (font.bpp) {
        case 1: drawBitmap1bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap); break;
        case 4: drawBitmap4bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap); break;
        }
        x += g.xAdvance;
    }
}

int Canvas::textWidth(const char *str) {
    const auto &font = bitmapFont(_font);
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

int Canvas::textHeight(const char *str) {
    const auto &font = bitmapFont(_font);
    int height = bitmapFontHeight(_font);

    while (*str != '\0') {
        auto c = *str++;
        if (c == '\n') {
            height += font.yAdvance;
        }
    }

    return height;
}

