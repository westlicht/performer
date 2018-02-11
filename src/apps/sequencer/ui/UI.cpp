#include "Config.h"

#include "UI.h"
#include "Key.h"

#include "core/Debug.h"
#include "core/profiler/Profiler.h"
#include "core/utils/StringBuilder.h"

#include "model/Model.h"

UI::UI(Model &model, Engine &engine, LCD &lcd, ButtonLedMatrix &blm, Encoder &encoder) :
    _model(model),
    _engine(engine),
    _lcd(lcd),
    _blm(blm),
    _encoder(encoder),
    _frameBuffer(CONFIG_LCD_WIDTH, CONFIG_LCD_HEIGHT, _frameBufferData),
    _canvas(_frameBuffer),
    _pageManager(_pages),
    _pageContext({ _messageManager, _keyState, _model, _engine }),
    _pages(_pageManager, _pageContext)
{
}

void UI::init() {
    _pageManager.push(&_pages.top);
#ifdef CONFIG_ENABLE_ASTEROIDS
    _pageManager.push(&_pages.asteroids);
#else
    _pageManager.push(&_pages.noteSequence);
#endif

    _engine.setMessageHandler([this] (const char *text, uint32_t duration) {
        _messageManager.showMessage(text, duration);
    });

    _lastUpdateTicks = os::ticks();
}

void UI::update() {
    handleKeys();
    handleEncoder();

    _pageManager.updateLeds(_leds);
    _blm.setLeds(_leds.array());

    // update display at target fps
    uint32_t currentTicks = os::ticks();
    uint32_t intervalTicks = os::time::ms(1000 / _pageManager.fps());
    if (currentTicks - _lastUpdateTicks >= intervalTicks) {
        _pageManager.draw(_canvas);
        _messageManager.update();
        _messageManager.draw(_canvas);
        _lcd.draw(_frameBuffer.data());
        _lastUpdateTicks += intervalTicks;
    }
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
            _pageManager.dispatchEvent(keyEvent);
            break;
        }
        }
    }
}
