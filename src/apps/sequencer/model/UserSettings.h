#pragma once

#include <core/io/VersionedSerializedWriter.h>
#include <core/io/VersionedSerializedReader.h>
#include <core/gfx/Brightness.h>
#include <unordered_map>
#include <vector>

//const char *SettingBrightness   = "brightness";
//const char *SettingScreensaver  = "screensaver";
#define SettingBrightness "brightness"
#define SettingScreensaver "screensaver"

#include <iostream>

class Setting {
public:
    Setting(
        std::string key,
        std::string menuItem,
        std::vector<std::string> menuItemKeys,
        std::vector<int> menuItemValues,
        int defaultValue
    ) :
        _value(defaultValue),
        _key(std::move(key)),
        _menuItem(std::move(menuItem)),
        _menuItemKeys(std::move(menuItemKeys)),
        _menuItemValues(std::move(menuItemValues)),
        _defaultValue(defaultValue)
    {}

    std::string getKey() {
        return _key;
    }

    std::string getMenuItem() {
        return _menuItem;
    }

    std::string getMenuItemKey() {
        return _menuItemKeys[getCurrentIndex()];
    }

    void setValue(int index) {
        std::cout << "Set index: " << index << std::endl;
        if (index < 0) index = 0;
        if (index > _menuItemValues.size() - 1) index = _menuItemValues.size() - 1;
        _value = _menuItemValues[index];
    };

    void shiftValue(int shift) {
        setValue(getCurrentIndex() + shift);
    };

    int &getValue() {
        return _value;
    };

    const int &getValue() const {
        return _value;
    }

    void reset() {
        _value = _defaultValue;
    };

private:
    int getCurrentIndex() {
        auto it = std::find(_menuItemValues.begin(), _menuItemValues.end(), _value);
        return std::distance(_menuItemValues.begin(), it);
    }

    int _value;

    std::string _key;
    std::string _menuItem;
    std::vector<std::string> _menuItemKeys;
    std::vector<int> _menuItemValues;
    int _defaultValue;
};

class BrightnessSetting : public Setting {
public:
    BrightnessSetting() : Setting(
            SettingBrightness,
            "Brightness",
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"},
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
            10
    ) {}
};

class ScreensaverSetting : public Setting {
public:
    ScreensaverSetting() : Setting(
            SettingScreensaver,
            "Screensaver",
            {"off", "30s", "1m", "10m", "30m"},
            {-1, 30, 60, 600, 1800},
            -1
    ) {}
};

class UserSettings {
public:
    UserSettings() {
        addSetting(BrightnessSetting());
        addSetting(ScreensaverSetting());
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void set(int key, int value);
    void shift(int key, int shift);
    Setting *get(int key);
    Setting *get(const char *key);
    std::vector<Setting> all();

    void clear();
    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    std::vector<Setting> _settings;

    void addSetting(const Setting& setting) {
        _settings.push_back(setting);
    }
};
