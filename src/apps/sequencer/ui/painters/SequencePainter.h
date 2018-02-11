#pragma once

#include "core/gfx/Canvas.h"

class SequencePainter {
public:
    static void drawProbability(Canvas &canvas, int x, int y, int w, int h, int probability, int maxProbability);
    static void drawLength(Canvas &canvas, int x, int y, int w, int h, int length, int maxLength);
    static void drawLengthRange(Canvas &canvas, int x, int y, int w, int h, int length, int range, int maxLength);
};
