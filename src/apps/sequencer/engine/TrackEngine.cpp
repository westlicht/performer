#include "TrackEngine.h"

#include "core/Debug.h"

void TrackEngine::setSwing(int swing) {
    _swing = swing;
}

void TrackEngine::setPattern(int pattern) {
    _pattern = pattern;
    changePattern();
}

void TrackEngine::setMute(bool mute) {
    _mute = mute;
}

void TrackEngine::setFill(bool fill) {
    _fill = fill;
}
