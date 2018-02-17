#pragma once

#include "ListPage.h"

#include "ui/model/ProjectSelectListModel.h"

class ProjectSelectPage : public ListPage {
public:
    ProjectSelectPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool, int)> ResultCallback;

    using ListPage::show;
    void show(const char *title, int selectedSlot, bool allowEmpty, ResultCallback callback);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void closeWithResult(bool result);

    const char *_title;
    bool _allowEmpty;
    ResultCallback _callback;

    ProjectSelectListModel _listModel;
};
