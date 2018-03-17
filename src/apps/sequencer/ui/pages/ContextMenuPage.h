#pragma once

#include "BasePage.h"

#include "ui/model/ContextMenuModel.h"

class ContextMenuPage : public BasePage {
public:
    ContextMenuPage(PageManager &manager, PageContext &context);

    typedef std::function<void(int)> ResultCallback;

    using BasePage::show;
    void show(ContextMenuModel &contextMenuModel, ResultCallback callback);

    virtual void draw(Canvas &canvas) override;

    virtual bool isModal() const override { return true; }

    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

protected:
    void closeAndCallback(int index);

    ContextMenuModel *_contextMenuModel;
    ResultCallback _callback;
};
