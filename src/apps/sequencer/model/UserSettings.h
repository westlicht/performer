#pragma once

#include <core/io/VersionedSerializedWriter.h>
#include <core/io/VersionedSerializedReader.h>

const int MinBrightness = 1;
const int MaxBrightness = 10;
const int DefaultBrightness = MaxBrightness;

class UserSettings {
public:
    UserSettings() :
        _brightness(DefaultBrightness)
    {}

    //----------------------------------------
    // Brightness
    //----------------------------------------

    void setBrightness(int brightness);
    void shiftBrightness(int shift);
    int getBrightness();

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();
    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    int _brightness;
};
