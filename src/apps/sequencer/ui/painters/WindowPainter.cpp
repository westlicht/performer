#include "WindowPainter.h"

#include "Config.h"

#include "ui/layouts/PartitionLayout.h"

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
    canvas.drawText(x + (w - canvas.textWidth(text)) / 2 - 1, PageHeight - 2, text);
}
