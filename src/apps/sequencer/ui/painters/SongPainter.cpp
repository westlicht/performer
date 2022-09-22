#include "SongPainter.h"

void SongPainter::drawArrowDown(Canvas &canvas, int x, int y, int w) {
    x += w / 2;
    canvas.vline(x, y, 2);
    canvas.point(x - 1, y);
    canvas.point(x + 1, y);
}

void SongPainter::drawArrowUp(Canvas &canvas, int x, int y, int w) {
    x += w / 2;
    canvas.vline(x, y, 2);
    canvas.point(x - 1, y + 1);
    canvas.point(x + 1, y + 1);
}

void SongPainter::drawArrowRight(Canvas &canvas, int x, int y, int w, int h) {
    x += w / 2;
    y += h / 2;
    canvas.hline(x, y, 2);
    canvas.point(x, y - 1);
    canvas.point(x, y + 1);
}

void SongPainter::drawProgress(Canvas &canvas, int x, int y, int w, int h, float progress) {
    int pw = w * progress;
    canvas.setColor(Color::Medium);
    canvas.fillRect(x + pw, y, w - pw, h);
    canvas.setColor(Color::Bright);
    canvas.fillRect(x, y, pw, h);
}
