#pragma once

#include "BasePage.h"
#include "ContextMenu.h"

class PatternPage : public BasePage {
public:
    PatternPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void contextAction(int index);
    bool contextActionEnabled(int index) const;
    void initPattern();
    void copyPattern();
    void pastePattern();
    void duplicatePattern();

    ContextMenu _contextMenu;
};
