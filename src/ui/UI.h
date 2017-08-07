#pragma once

#include "Config.h"
#include "Page.h"
#include "Key.h"
#include "Leds.h"

#include "pages/Pages.h"

#include "drivers/ButtonLedMatrix.h"
#include "drivers/LCD.h"

#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"

#include "engine/Engine.h"

#include "model/Model.h"

class Key;

class UI {
public:
    UI(Model &model, Engine &engine, LCD &lcd, ButtonLedMatrix &blm);

    void init();
    void update();

private:
    void handleKeys();

    Model &_model;
    Engine &_engine;

    LCD &_lcd;
    ButtonLedMatrix &_blm;

    uint8_t _frameBufferData[CONFIG_LCD_WIDTH * CONFIG_LCD_HEIGHT];
    FrameBuffer8bit _frameBuffer;
    Canvas _canvas;

    KeyState _keyState;
    Leds _leds;

    PageManager _pageManager;
    Pages _pages;
};