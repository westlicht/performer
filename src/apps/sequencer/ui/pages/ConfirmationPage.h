#pragma once

#include "BasePage.h"

#include <functional>

class ConfirmationPage : public BasePage {
public:
    ConfirmationPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool)> ResultCallback;

    using BasePage::show;
    void show(const char *text, ResultCallback callback);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void closeWithResult(bool result);

    const char *_text;
    ResultCallback _callback;
};
