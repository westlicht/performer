#pragma once

#include "core/math/Math.h"

#include <cstdint>

class NudgeTempo {
public:
    NudgeTempo() {
    }

    void setDirection(int direction) {
        _direction = direction;
    }

    float strength() const {
        return _strength;
    }

    void update(float dt) {
        if (_direction == 0) {
            _strength = 0.f;
        } else {
            _strength = clamp(_strength + _direction * dt, -1.f, 1.f);
        }
    }

private:
    int8_t _direction = 0;
    float _strength = 0.f;
};
