#pragma once

#include "BasePage.h"

#include <functional>

class ConfirmationPage : public BasePage {
public:
    ConfirmationPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool)> ResultCallback;

    void show(const char *text, ResultCallback callback);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void close(bool result);

    const char *_text;
    ResultCallback _callback;
};
