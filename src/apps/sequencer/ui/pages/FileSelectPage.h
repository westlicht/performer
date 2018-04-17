#pragma once

#include "ListPage.h"

#include "ui/model/FileSelectListModel.h"

#include "model/FileManager.h"

class FileSelectPage : public ListPage {
public:
    FileSelectPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool, int)> ResultCallback;

    using ListPage::show;
    void show(const char *title, FileType type, int selectedSlot, bool allowEmpty, ResultCallback callback);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void closeWithResult(bool result);

    const char *_title;
    FileType _type;
    bool _allowEmpty;
    ResultCallback _callback;

    FileSelectListModel _listModel;
};
