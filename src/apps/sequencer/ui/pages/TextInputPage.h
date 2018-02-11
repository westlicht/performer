#pragma once

#include "BasePage.h"


class TextInputPage : public BasePage {
public:
    TextInputPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void clear();
    void insert(char c);
    void backspace();
    void del();
    void moveLeft();
    void moveRight();

    const char *_title = "PROJECT NAME:";
    char _text[5];
    int _maxTextLength = 4;;

    int _selectedIndex;
    int _cursorIndex;
};
