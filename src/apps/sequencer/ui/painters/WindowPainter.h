#pragma once

#include "Config.h"

#include "model/Model.h"

#include "engine/Engine.h"

#include "ui/Key.h"

#include "core/gfx/Canvas.h"

class WindowPainter {
public:
    static void clear(Canvas &canvas);

    static void drawFrame(Canvas &canvas, int x, int y, int w, int h);

    static void drawFunctionKeys(Canvas &canvas, const char *names[], KeyState &keyState);

    static void drawClock(Canvas &canvas, Engine &engine);
    static void drawActiveTrack(Canvas &canvas, int track);
    static void drawActivePattern(Canvas &canvas, int edit, int play);
    static void drawActiveMode(Canvas &canvas, const char *mode);
    static void drawActiveFunction(Canvas &canvas, const char *function);

    static void drawHeader(Canvas &canvas, Model &model, Engine &engine, const char *mode);

private:
    const static int PageWidth = CONFIG_LCD_WIDTH;
    const static int PageHeight = CONFIG_LCD_HEIGHT;
    const static int HeaderHeight = 9;
    const static int FooterHeight = 9;
    const static int FunctionKeyCount = CONFIG_FUNCTION_KEY_COUNT;
    const static int FunctionKeyWidth = 40;
    const static int FunctionKeyHeight = 8;
};
