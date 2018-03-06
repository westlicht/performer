#pragma once

#include "BasePage.h"

class MonitorPage : public BasePage {
public:
    MonitorPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void drawCvIn(Canvas &canvas);
    void drawCvOut(Canvas &canvas);

    enum class Mode : uint8_t {
        CvIn,
        CvOut,
    };

    Mode _mode = Mode::CvIn;
};
