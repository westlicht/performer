#pragma once

#include "Config.h"
#include "MessageManager.h"
#include "Page.h"
#include "PageManager.h"
#include "Key.h"
#include "KeyPressEventTracker.h"
#include "Leds.h"

#include "pages/Pages.h"

#include "drivers/ButtonLedMatrix.h"
#include "drivers/Encoder.h"
#include "drivers/LCD.h"

#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"

#include "engine/Engine.h"

#include "model/Model.h"

class Key;

class UI {
public:
    UI(Model &model, Engine &engine, LCD &lcd, ButtonLedMatrix &blm, Encoder &encoder);

    void init();
    void update();

private:
    void handleKeys();
    void handleEncoder();

    Model &_model;
    Engine &_engine;

    LCD &_lcd;
    ButtonLedMatrix &_blm;
    Encoder &_encoder;

    uint8_t _frameBufferData[CONFIG_LCD_WIDTH * CONFIG_LCD_HEIGHT];
    FrameBuffer8bit _frameBuffer;
    Canvas _canvas;
    uint32_t _lastUpdateTicks;

    KeyState _keyState;
    KeyPressEventTracker _keyPressEventTracker;
    Leds _leds;

    MessageManager _messageManager;

    PageManager _pageManager;
    PageContext _pageContext;
    Pages _pages;
};
