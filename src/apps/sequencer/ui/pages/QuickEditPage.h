#pragma once

#include "BasePage.h"

#include "ui/model/ListModel.h"

class QuickEditPage : public BasePage {
public:
    QuickEditPage(PageManager &manager, PageContext &context);

    using BasePage::show;
    void show(ListModel &listModel, int row);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    ListModel *_listModel;
    int _row;
};
