#pragma once

#include "Config.h"

#include "engine/Engine.h"

#include "core/gfx/Canvas.h"

class WindowPainter {
public:
    static void drawFrame(Canvas &canvas, int x, int y, int w, int h);

    static void drawFunctionKey(Canvas &canvas, int index, const char *text, bool pressed);

    static void drawClock(Canvas &canvas, Engine &engine);

private:
    const static int PageWidth = CONFIG_LCD_WIDTH;
    const static int PageHeight = CONFIG_LCD_HEIGHT;
    const static int FunctionKeyCount = CONFIG_FUNCTION_KEY_COUNT;
    const static int FunctionKeyWidth = 40;
    const static int FunctionKeyHeight = 8;
};
