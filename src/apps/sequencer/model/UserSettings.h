#pragma once

#include <core/io/VersionedSerializedWriter.h>
#include <core/io/VersionedSerializedReader.h>
#include <unordered_map>
#include <vector>

#include <iostream>

#define SettingBrightness "brightness"
#define SettingScreensaver "screensaver"

class BaseSetting {
public:
    virtual std::string getKey() = 0;
    virtual void shiftValue(int shift) = 0;
    virtual void setValue(int value) = 0;
    virtual std::string getMenuItem() = 0;
    virtual std::string getMenuItemKey() = 0;
    virtual void read(VersionedSerializedReader &writer) = 0;
    virtual void write(VersionedSerializedWriter &writer) = 0;
    virtual void reset() = 0;
};

template<typename T>
class Setting : public BaseSetting {
public:
    Setting(
        std::string key,
        std::string menuItem,
        std::vector<std::string> menuItemKeys,
        std::vector<T> menuItemValues,
        T defaultValue
    ) :
        _value(defaultValue),
        _key(std::move(key)),
        _menuItem(std::move(menuItem)),
        _menuItemKeys(std::move(menuItemKeys)),
        _menuItemValues(std::move(menuItemValues)),
        _defaultValue(defaultValue)
    {}

    std::string getKey() override {
        return _key;
    }

    std::string getMenuItem() override {
        return _menuItem;
    }

    std::string getMenuItemKey() override {
        return _menuItemKeys[getCurrentIndex()];
    }

    void setValue(int index) override {
        if (index < 0) index = 0;
        if (index > _menuItemValues.size() - 1) index = _menuItemValues.size() - 1;
        std::cout << "Set index: " << index << ": " << _menuItemValues[index] << std::endl;
        _value = _menuItemValues[index];
    };

    void shiftValue(int shift) override {
        setValue(getCurrentIndex() + shift);
    };

    T &getValue() {
        std::cout<<_value<<std::endl;
        return _value;
    };

    const T &getValue() const {
        std::cout<<_value<<std::endl;
        return _value;
    }

    void reset() override {
        _value = _defaultValue;
    };

    void read(VersionedSerializedReader &reader) override {
        reader.read(getValue());
    };

    void write(VersionedSerializedWriter &writer) override {
        writer.write(getValue());
    };

private:
    int getCurrentIndex() {
        auto it = std::find(_menuItemValues.begin(), _menuItemValues.end(), _value);
        return std::distance(_menuItemValues.begin(), it);
    }

    T _value;

    std::string _key;
    std::string _menuItem;
    std::vector<std::string> _menuItemKeys;
    std::vector<T> _menuItemValues;
    T _defaultValue;
};

class BrightnessSetting : public Setting<float> {
public:
    BrightnessSetting() : Setting(
            SettingBrightness,
            "Brightness",
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"},
            {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0},
            1.0
    ) {}
};

class ScreensaverSetting : public Setting<int> {
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
        addSetting(new BrightnessSetting());
        addSetting(new ScreensaverSetting());
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void set(int key, int value);
    void shift(int key, int shift);
    BaseSetting *get(int key);
    template<typename S>
    S *get(const char *key) { return dynamic_cast<S *>(_get(key)); }
    std::vector<BaseSetting *> all();

    void clear();
    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    std::vector<BaseSetting *> _settings;

    template<typename T>
    void addSetting(Setting<T> *setting) {
        _settings.push_back(setting);
    }
    BaseSetting *_get(const char *key);
};
