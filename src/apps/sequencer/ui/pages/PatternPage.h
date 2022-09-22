#pragma once

#include "BasePage.h"

class PatternPage : public BasePage {
public:
    PatternPage(PageManager &manager, PageContext &context);

    void setModal(bool modal) { _modal = modal; }

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return _modal; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initPattern();
    void copyPattern();
    void pastePattern();
    void duplicatePattern();
    void sendMidiProgramSave();

    bool _modal = false;
    bool _latching = false;
    bool _syncing = false;
    int8_t _snapshotTargetPattern = -1;
};
