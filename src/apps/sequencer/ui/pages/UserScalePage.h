#pragma once

#include "ListPage.h"

#include "ui/model/UserScaleListModel.h"

// #include "model/Scale.h"

class UserScalePage : public ListPage {
public:
    UserScalePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void setSelectedIndex(int index);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;
    void initUserScale();
    void copyUserScale();
    void pasteUserScale();

    int _selectedIndex;
    UserScale *_userScale;
    UserScaleListModel _listModel;
};
