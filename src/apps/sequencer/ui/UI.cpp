#include "UI.h"
#include "Key.h"

#include "core/Debug.h"
#include "core/profiler/Profiler.h"
#include "core/utils/StringBuilder.h"

#include "model/Model.h"

PROFILER_INTERVAL(UIDraw, "UI.draw")
PROFILER_INTERVAL(UIInterval, "UI.interval")

UI::UI(Model &model, Engine &engine, LCD &lcd, ButtonLedMatrix &blm, Encoder &encoder) :
    _model(model),
    _engine(engine),
    _lcd(lcd),
    _blm(blm),
    _encoder(encoder),
    _frameBuffer(CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT, _frameBufferData),
    _canvas(_frameBuffer),
    _pageManager(_pages),
    _pageContext({ _keyState, _model, _engine }),
    _pages(_pageManager, _pageContext)
{
    _pageManager.push(&_pages.top);
    _pageManager.push(&_pages.track);
}

void UI::init() {
    _canvas.setColor(0xf);
    _canvas.drawText(10, 30, "Hello World!");
    _lcd.draw(_frameBuffer.data());
}

void UI::update() {
    handleKeys();
    handleEncoder();

    _pageManager.updateLeds(_leds);
    _blm.setLeds(_leds.array());

    static int counter = 0;
    if (counter % 20 == 0) {
        PROFILER_INTERVAL_END(UIInterval);
        PROFILER_INTERVAL_BEGIN(UIInterval);
        PROFILER_INTERVAL_BEGIN(UIDraw);

        _pageManager.draw(_canvas);
        _lcd.draw(_frameBuffer.data());

        PROFILER_INTERVAL_END(UIDraw);
    }
    ++counter;
}

void UI::handleKeys() {
    ButtonLedMatrix::Event event;
    while (_blm.nextEvent(event)) {
        bool isDown = event.action() == ButtonLedMatrix::Event::KeyDown;
        _keyState[event.value()] = isDown;
        KeyEvent keyEvent(isDown ? Event::KeyDown : Event::KeyUp, Key(event.value(), _keyState));
        _pageManager.dispatchEvent(keyEvent);
    }
}

void UI::handleEncoder() {
    Encoder::Event event;
    while (_encoder.nextEvent(event)) {
        switch (event) {
        case Encoder::Left:
        case Encoder::Right: {
            EncoderEvent encoderEvent(Event::Encoder, event == Encoder::Left ? -1 : 1, _keyState[Key::Encoder]);
            _pageManager.dispatchEvent(encoderEvent);
            break;
        }
        case Encoder::Down:
        case Encoder::Up: {
            bool isDown = event == Encoder::Down;
            _keyState[Key::Encoder] = isDown ? 1 : 0;
            KeyEvent keyEvent(isDown ? Event::KeyDown : Event::KeyUp, Key(Key::Code::Encoder, _keyState));
            break;
        }
        }
    }
}
