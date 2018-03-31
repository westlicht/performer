#pragma once

#include "BasePage.h"

#include "ui/model/ListModel.h"

class ListPage : public BasePage {
public:
    ListPage(PageManager &manager, PageContext &context, ListModel &listModel);

    void setListModel(ListModel &listModel);

    virtual void show() override;

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

    int selectedRow() const { return _selectedRow; }
    void setSelectedRow(int selectedRow);

    bool edit() const { return _edit; }
    void setEdit(bool edit) { _edit = edit; }

    void setTopRow(int row);

protected:
    virtual void drawCell(Canvas &canvas, int row, int column, int x, int y, int w, int h);

private:
    void scrollTo(int row);

    static constexpr int LineHeight = 10;
    static constexpr int LineCount = 4;

    ListModel *_listModel;
    int _selectedRow = 0;
    int _displayRow = 0;
    bool _edit = false;
};
