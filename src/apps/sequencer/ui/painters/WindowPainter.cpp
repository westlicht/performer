#include "WindowPainter.h"

#include "Config.h"

#include "core/utils/StringBuilder.h"

#include "ui/layouts/PartitionLayout.h"

void WindowPainter::clear(Canvas &canvas) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0);
    canvas.fill();
}

void WindowPainter::drawFrame(Canvas &canvas, int x, int y, int w, int h) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0);
    canvas.fillRect(x, y, w, h);
    canvas.setColor(0xf);
    canvas.drawRect(x, y, w, h);
}

void WindowPainter::drawFunctionKeys(Canvas &canvas, const char *names[], KeyState &keyState) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0x7);
    canvas.hline(0, PageHeight - FooterHeight - 1, PageWidth);

    for (int i = 0; i < FunctionKeyCount; ++i) {
        if (names[i] || (i + 1 < FunctionKeyCount && names[i + 1])) {
            int x = (PageWidth * (i + 1)) / FunctionKeyCount;
            canvas.vline(x, PageHeight - FooterHeight, FooterHeight);
        }
    }

    canvas.setFont(Font::Tiny);
    canvas.setColor(0xf);

    for (int i = 0; i < FunctionKeyCount; ++i) {
        if (names[i]) {
            bool pressed = keyState[Key::F0 + i];

            int x0 = (PageWidth * i) / FunctionKeyCount;
            int x1 = (PageWidth * (i + 1)) / FunctionKeyCount;
            int w = x1 - x0 + 1;

            canvas.setBlendMode(BlendMode::Set);

            if (pressed) {
                canvas.fillRect(x0, PageHeight - FooterHeight, w, FooterHeight);
                canvas.setBlendMode(BlendMode::Sub);
            }

            canvas.drawText(x0 + (w - canvas.textWidth(names[i])) / 2, PageHeight - 3, names[i]);
        }
    }
}

void WindowPainter::drawClock(Canvas &canvas, Engine &engine) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.fillRect(1, 1, 7, 7);

    canvas.setBlendMode(BlendMode::Sub);
    canvas.setColor(0xf);
    static const char *clockModeName[] = { "A", "M", "S" };
    canvas.drawText(2, 8 - 2, clockModeName[engine.clock().activeMode()]);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.drawText(10, 8 - 2, FixedStringBuilder<8>("%.1f", engine.bpm()));
}

void WindowPainter::drawActiveTrack(Canvas &canvas, int track) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.drawText(48, 8 - 2, FixedStringBuilder<8>("T%d", track + 1));
}

void WindowPainter::drawActivePattern(Canvas &canvas, int edit, int play) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.drawText(64, 8 - 2, FixedStringBuilder<8>("P%d", edit + 1));
}

void WindowPainter::drawActiveMode(Canvas &canvas, const char *mode) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.drawText(PageWidth - canvas.textWidth(mode) - 2, 8 - 2, mode);
}

void WindowPainter::drawActiveFunction(Canvas &canvas, const char *function) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.drawText(96, 8 - 2, function);
}

void WindowPainter::drawHeader(Canvas &canvas, Model &model, Engine &engine, const char *mode) {
    drawClock(canvas, engine);
    drawActiveTrack(canvas, model.project().selectedTrackIndex());
    drawActivePattern(canvas, model.project().selectedPatternIndex(), 0);
    drawActiveMode(canvas, mode);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0x7);
    canvas.hline(0, HeaderHeight, PageWidth);
}
