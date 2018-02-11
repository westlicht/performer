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

void WindowPainter::drawFunctionKey(Canvas &canvas, int index, const char *text, bool pressed) {
    PartitionLayout<
        PageWidth,
        FunctionKeyCount,
        FunctionKeyWidth
    > partitionLayout;

    int x = partitionLayout.cellOffset(index);
    int w = partitionLayout.cellSize();
    int h = FunctionKeyHeight;

    canvas.setColor(0xf);
    canvas.setBlendMode(BlendMode::Set);
    canvas.vline(x, PageHeight - h + 1, h - 1);
    canvas.vline(x + w - 1, PageHeight - h + 1, h - 1);
    canvas.hline(x + 1, PageHeight - h, w - 2);

    if (pressed) {
        // canvas.setColor(0xf);
        canvas.fillRect(x + 1, PageHeight - h + 1, w - 2, h - 1);
    }

    // canvas.setColor(pressed ? 0x7 : 0xf);
    canvas.setBlendMode(pressed ? BlendMode::Sub : BlendMode::Set);
    canvas.setFont(Font::Tiny);
    canvas.drawText(x + (w - canvas.textWidth(text)) / 2, PageHeight - 2, text);
}

void WindowPainter::drawFunctionKeys(Canvas &canvas, const char *names[], KeyState &keyState) {
    for (int i = 0; i < 5; ++i) {
        if (names[i]) {
            drawFunctionKey(canvas, i, names[i], keyState[Key::F0 + i]);
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
    canvas.drawText(10, 8 - 2, FixedStringBuilder<16>("%.1f", engine.bpm()));
}

void WindowPainter::drawActiveTrack(Canvas &canvas, int track) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.drawText(48, 8 - 2, FixedStringBuilder<16>("TRACK%d", track + 1));
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
    drawActiveMode(canvas, mode);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0x7);
    canvas.hline(0, 9, PageWidth);
}
