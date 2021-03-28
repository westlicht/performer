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

bool Screensaver::consumeKey(Key key, bool isDown) {
    _buttonPressed = isDown;
//    bool consume = _screenSaved;
    off();
//    if (consume) {
//        switch(key.code()) {
//            case Key::Code::Play:
//            case Key::Code::Tempo:
//            case Key::Code::Pattern:
//            case Key::Code::Performer:
//            case Key::Code::Shift:
//            case Key::Code::Page:
//            case Key::Code::Track0 ... Key::Code::Track7:
//                return false;
//            default:
//                return true;
//        }
//    } else {
//        return false;
//    }
    // Probably don't want to consume anything
    return false;
}

bool Screensaver::consumeEncoder(bool isDown) {
    _buttonPressed = isDown;
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