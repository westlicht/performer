#pragma once

#include "BasePage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

class ValuePage : public BasePage {
public:
    struct Handler {
        Key::Code exitKey;
        std::function<void(StringBuilder &)> value;
        std::function<void(int, bool, bool)> encoder;
    };

    ValuePage(PageManager &manager, PageContext &context);

    void show(const Handler &handler);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    Handler _handler;
};
