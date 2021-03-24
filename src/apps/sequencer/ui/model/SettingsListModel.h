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
        return 1;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (row == 0) { // TODO proper map of settings
            if (column == 0) {
                str("Brightness");
            } else if (column == 1) {
                str("%i", _userSettings.getBrightness());
            }
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            if (row == 0) { // TODO proper map of settings
                std::cout << "edit Column 1" << value << " " << shift << std::endl;
                _userSettings.shiftBrightness(value);
            }
        }
    }

private:
    UserSettings &_userSettings;
};
