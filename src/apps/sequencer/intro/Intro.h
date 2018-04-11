#pragma once

#include "core/gfx/Canvas.h"

class Intro {
public:
    Intro();

    void init();

    void update(float dt);
    void draw(Canvas &canvas);

private:
    float _time;
};
