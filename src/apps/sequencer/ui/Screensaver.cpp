#include "Screensaver.h"

void Screensaver::on() {
    _screenSaved = true;
    _canvas.screensaver();
}

void Screensaver::off() {
    _screenSaved = false;
    setScreenOnTicks(0);
}

bool Screensaver::shouldBeOn() {
    // TODO Is tick comparison correct? Seems roughly correct...
    return _screenOffAfter > 0 && !_buttonPressed && _screenOnTicks > _screenOffAfter;
}

void Screensaver::consumeKey(KeyEvent &event) {
    consumeKey(event, event.key());
}

void Screensaver::consumeKey(KeyPressEvent &event) {
    consumeKey(event, event.key());
}

void Screensaver::consumeKey(Event &event, Key key) {
    if (_screenSaved && key.code() == Key::Code::Encoder) {
        event.consume();
    }

    if (_wakeMode == 1 && _screenSaved) { // required
        switch(key.code()) {
            case Key::Code::Play:
            case Key::Code::Shift:
            case Key::Code::Left:
            case Key::Code::Right:
            case Key::Code::Track0 ... Key::Code::Track7:
            case Key::Code::Step0 ... Key::Code::Step7:
            case Key::Code::Step8 ... Key::Code::Step15:
                return;
        }
    }

    // Don't turn on screensaver if button is held
    if (event.type() == Event::Type::KeyDown) {
        _buttonPressed = true;
    } else if (event.type() == Event::Type::KeyUp) {
        _buttonPressed = false;
    }

    if (event.type() == Event::Type::KeyPress) {
        off();
    }
}

void Screensaver::consumeEncoder(EncoderEvent &event) {
    if (_screenSaved) {
        // Don't really want this since it disables
        // setting on all steps with the encoder when the screen is saved
        // Downside is that we could modify menu items for example
        // when the screen is off/first comes on
//        event.consume();

        if (_wakeMode == 1) { // required
            return;
        }
    }

    off();
}

void Screensaver::setScreenOnTicks(uint32_t ticks) {
    _screenOnTicks = ticks;
}

void Screensaver::incScreenOnTicks(uint32_t ticks) {
    if (_screenOffAfter > 0) { // Prevent flickering when setting screensaver in settings
        _screenOnTicks += ticks;
    }
}