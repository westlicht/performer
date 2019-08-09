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

    static void drawFunctionKeys(Canvas &canvas, const char *names[], const KeyState &keyState, int highlight = -1);

    static void drawClock(Canvas &canvas, const Engine &engine);
    static void drawActiveState(Canvas &canvas, int track, int playPattern, int editPattern, bool snapshotActive, bool songActive);
    static void drawActiveMode(Canvas &canvas, const char *mode);
    static void drawActiveFunction(Canvas &canvas, const char *function);

    static void drawHeader(Canvas &canvas, const Model &model, const Engine &engine, const char *mode);
    static void drawFooter(Canvas &canvas);
    static void drawFooter(Canvas &canvas, const char *names[], const KeyState &keyState, int highlight = -1);

    static void drawScrollbar(Canvas &canvas, int x, int y, int w, int h, int totalRows, int visibleRows, int displayRow);

private:
    const static int PageWidth = CONFIG_LCD_WIDTH;
    const static int PageHeight = CONFIG_LCD_HEIGHT;
    const static int HeaderHeight = 9;
    const static int FooterHeight = 9;
    const static int FunctionKeyCount = CONFIG_FUNCTION_KEY_COUNT;
    const static int FunctionKeyWidth = 40;
    const static int FunctionKeyHeight = 8;
};
