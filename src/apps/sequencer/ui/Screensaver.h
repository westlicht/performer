#pragma once

#include <cstdint>
#include <core/gfx/Canvas.h>
#include "Key.h"

class Screensaver {
public:
    Screensaver(Canvas &canvas, uint32_t &screenOffAfter) :
        _canvas(canvas),
        _screenOffAfter(screenOffAfter)
    {}

    void on();
    void off();
    bool shouldBeOn();

    void setScreenOnTicks(uint32_t ticks);
    void incScreenOnTicks(uint32_t ticks);

    bool consumeKey(Key key, bool isDown);
    bool consumeEncoder(bool isDown);

private:
    Canvas &_canvas;
    bool _screenSaved;
    bool _buttonPressed;
    uint32_t _screenOnTicks;
    uint32_t &_screenOffAfter;
};