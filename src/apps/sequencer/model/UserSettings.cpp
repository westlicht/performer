#include "UserSettings.h"

void UserSettings::set(int key, int value) {
    _settings[key]->setValue(value);
}

void UserSettings::shift(int key, int shift) {
    _settings[key]->shiftValue(shift);
}

BaseSetting *UserSettings::_get(const std::string& key) {
    for (auto &setting : _settings) {
        if (setting->getKey() == key) {
            return setting;
        }
    }
    return nullptr;
}

BaseSetting *UserSettings::get(int key) {
    return _settings[key];
}

std::vector<BaseSetting *> UserSettings::all() {
    return _settings;
}

void UserSettings::clear() {
    for (auto &setting : _settings) {
        setting->reset();
    }
}

void UserSettings::write(VersionedSerializedWriter &writer) const {
    for (auto &setting : _settings) {
        setting->write(writer);
    }
}

void UserSettings::read(VersionedSerializedReader &reader) {
    for (auto &setting : _settings) {
        setting->read(reader);
    }
}