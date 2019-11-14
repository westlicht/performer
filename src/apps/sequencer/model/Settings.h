#pragma once

#include "Calibration.h"
#include "Serialize.h"

class Settings {
public:
    static constexpr uint32_t Version = 1;

    static const char *Filename;

    Settings();

    const Calibration &calibration() const { return _calibration; }
          Calibration &calibration()       { return _calibration; }

    void clear();

    void write(VersionedSerializedWriter &writer) const;
    bool read(VersionedSerializedReader &reader);

    void writeToFlash() const;
    bool readFromFlash();

private:
    Calibration _calibration;
};
