#pragma once

#include "ListModel.h"

class UtilitiesListModel : public ListModel {
public:
    enum Item {
        FormatSdCard,
        Last
    };

    virtual int rows() const override {
        return Last;
    }

    virtual int columns() const override {
        return 1;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
    }

private:

    static const char *itemName(Item item) {
        switch (item) {
        case FormatSdCard:      return "Format SD card";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }
};
