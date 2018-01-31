#pragma once

#include "os/os.h"

#include "core/gfx/Canvas.h"

#include <cstdint>

class MessageManager {
public:
    MessageManager();

    void showMessage(const char *text, uint32_t duration = 1000);

    void update();

    void draw(Canvas &canvas);

private:
    char _text[64];
    uint32_t _timeout = 0;

    os::Mutex _mutex;
};
