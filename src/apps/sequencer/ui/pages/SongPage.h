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
    static const int SlotCount = CONFIG_SONG_SLOT_COUNT;
    static const int RowCount = 4;

    void setSelectedSlot(int slot);
    void moveSelectedSlot(int offset, bool moveSlot);
    void scrollTo(int row);

    uint8_t pressedTrackKeys() const;

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSong();

    enum class Mode : uint8_t {
        Idle,
        Chain
    };

    Mode _mode;
    int _selectedSlot = -1;
    int _displayRow = 0;
};
