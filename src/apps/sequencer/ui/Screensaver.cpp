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
    // TODO get time, not ticks
    return _screenOffAfter > 0 && !_buttonPressed && _screenOnTicks > _screenOffAfter * 1000;
}

bool Screensaver::consumeKey(Key key, bool isDown) {
    _buttonPressed = isDown;
    off();
    if (_screenSaved) {
        return key.code() != Key::Code::Play; // Allow play function when in screensaver
    } else {
        return false;
    }
}

bool Screensaver::consumeEncoder(bool isDown) {
    _buttonPressed = isDown;
    off();
    return _screenSaved;
}

void Screensaver::setScreenOnTicks(uint32_t ticks) {
    _screenOnTicks = ticks;
}

void Screensaver::incScreenOnTicks(uint32_t ticks) {
    if (_screenOffAfter > 0) { // Prevent flickering when setting screensaver in settings
        _screenOnTicks += ticks;
    }
}