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
