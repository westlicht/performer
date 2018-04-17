#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/UserScale.h"

class UserScaleListModel : public ListModel {
public:
    UserScaleListModel()
    {}

    void setUserScale(UserScale &userScale) {
        _userScale = &userScale;
    }

    virtual int rows() const override {
        return Item::Last + _userScale->size();
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (row < int(Item::Last)) {
            if (column == 0) {
                formatName(Item(row), str);
            } else if (column == 1) {
                formatValue(Item(row), str);
            }
        } else {
            int index = row - int(Item::Last);
            if (column == 0) {
                str("Item%d", index + 1);
            } else if (column == 1) {
                _userScale->printItem(index, str);
            }
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (row < int(Item::Last)) {
            if (column == 1) {
                editValue(Item(row), value, shift);
            }
        } else {
            int index = row - int(Item::Last);
            if (column == 1) {
                _userScale->editItem(index, value, shift);
            }
        }
    }

private:
    enum Item {
        Name,
        Mode,
        Size,
        Last
    };

    const char *itemName(Item item) const {
        switch (item) {
        case Name:  return "Name";
        case Mode:  return "Mode";
        case Size:  return "Size";
        case Last:  break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Name:
            str(_userScale->name());
            break;
        case Mode:
            _userScale->printMode(str);
            break;
        case Size:
            _userScale->printSize(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Name:
            break;
        case Mode:
            _userScale->editMode(value, shift);
            break;
        case Size:
            _userScale->editSize(value, shift);
            break;
        case Last:
            break;
        }
    }

    UserScale *_userScale;
};
