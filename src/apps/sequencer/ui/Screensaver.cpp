#include <platform/sim/drivers/Encoder.h>
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

bool Screensaver::consumeKey(ButtonLedMatrix::Event event, Key key) {
    if (_wakeMode == 1 && _screenSaved) { // required
        switch(key.code()) {
            case Key::Code::Play:
            case Key::Code::Shift:
            case Key::Code::Left:
            case Key::Code::Right:
            case Key::Code::Track0 ... Key::Code::Track7:
            case Key::Code::Step0 ... Key::Code::Step7:
            case Key::Code::Step8 ... Key::Code::Step15:
                return false;
        }
    }

    _buttonPressed = event.action() == ButtonLedMatrix::Event::KeyDown;
    off();
    return false;
}

bool Screensaver::consumeEncoder(Encoder::Event event) {
    if (_wakeMode == 1 && _screenSaved) { // required
        switch(event) {
            case Encoder::Left:
            case Encoder::Right:
                return false;
        }
    }

    _buttonPressed = event == Encoder::Down;
    bool consume = _screenSaved;
    off();
    return consume;
}

void Screensaver::setScreenOnTicks(uint32_t ticks) {
    _screenOnTicks = ticks;
}

void Screensaver::incScreenOnTicks(uint32_t ticks) {
    if (_screenOffAfter > 0) { // Prevent flickering when setting screensaver in settings
        _screenOnTicks += ticks;
    }
}