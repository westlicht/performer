#pragma once

#include "Config.h"

#include "drivers/ButtonLedMatrix.h"
#include "drivers/LCD.h"

#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"

#include "engine/Engine.h"

class Key;

class UI {
public:
    UI(Engine &engine, LCD &lcd, ButtonLedMatrix &blm);

    void init();
    void update();

private:
    void handleKeys();
    void keyDown(const Key &key);
    void keyUp(const Key &key);
    void encoder(int value);

    void updateLeds();

    Engine &_engine;

    LCD &_lcd;
    ButtonLedMatrix &_blm;

    uint8_t _frameBufferData[LCD_WIDTH * LCD_HEIGHT];
    FrameBuffer8bit _frameBuffer;
    Canvas _canvas;

    uint8_t _selectedTrack = 0;
    
};
