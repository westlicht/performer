#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/UserSettings.h"

class SettingsListModel : public ListModel {
public:
    SettingsListModel(UserSettings &userSettings) :
        _userSettings(userSettings)
    {}

    virtual int rows() const override {
        return _userSettings.all().size();
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str("%s", _userSettings.get(row)->getMenuItem().c_str());
        } else if (column == 1) {
            str("%s", _userSettings.get(row)->getMenuItemKey().c_str());
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            std::cout << "edit Column 1" << value << " " << shift << std::endl;
            _userSettings.shift(row, value);
        }
    }

private:
    UserSettings &_userSettings;
};
