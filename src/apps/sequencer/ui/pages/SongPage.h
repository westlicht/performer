#pragma once

#include "BasePage.h"

class SongPage : public BasePage {
public:
    SongPage(PageManager &manager, PageContext &context);

    void reset();

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    static const int SlotCount = 16;

    void setSelectedSlot(int slot);
    void moveSelectedSlot(int offset, bool moveSlot);

    enum class Mode : uint8_t {
        Idle,
        Chain
    };

    Mode _mode;
    int _selectedSlot = -1;
};
