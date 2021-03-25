#include <iostream> // TODO remove
#include "UserSettings.h"

void UserSettings::setBrightness(int brightness) {
    if (brightness >= MinBrightness && brightness <= MaxBrightness) {
        std::cout<<"setting brightness "<<brightness<<std::endl;
        _brightness = brightness;
    }
}

void UserSettings::shiftBrightness(int shift) {
    setBrightness(getBrightness() + shift);
}

int &UserSettings::getBrightness() {
    return _brightness;
}

void UserSettings::clear() {
    std::cout<<"clearing brightness"<<std::endl;
    setBrightness(DefaultBrightness);
}

void UserSettings::write(VersionedSerializedWriter &writer) const {
    writer.write(_brightness);
}

void UserSettings::read(VersionedSerializedReader &reader) {
    reader.read(_brightness);
}