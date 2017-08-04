#pragma once

#include "core/gfx/Canvas.h"

class WindowPainter {
public:
    static void drawFrame(Canvas &canvas, int x, int y, int w, int h);
};
