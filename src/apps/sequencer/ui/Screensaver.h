#pragma once

#include <cstdint>
#include <core/gfx/Canvas.h>
#include "Key.h"
#include "Event.h"

class Screensaver {
public:
    Screensaver(Canvas &canvas, uint32_t &screenOffAfter, int &wakeMode) :
        _canvas(canvas),
        _screenOffAfter(screenOffAfter),
        _wakeMode(wakeMode)
    {}

    void on();
    void off();
    bool shouldBeOn();

    void setScreenOnTicks(uint32_t ticks);
    void incScreenOnTicks(uint32_t ticks);

    void consumeKey(KeyEvent &event);
    void consumeKey(KeyPressEvent &event);
    void consumeEncoder(EncoderEvent &event);

private:
    void consumeKey(Event &event, Key key);

    Canvas &_canvas;
    bool _screenSaved;
    bool _buttonPressed;
    uint32_t _screenOnTicks;
    uint32_t &_screenOffAfter;
    int &_wakeMode;
};