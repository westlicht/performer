#pragma once

#include "Calibration.h"
#include "Serialize.h"
#include "FileDefs.h"
#include "FlashWriter.h"
#include "FlashReader.h"

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

    fs::Error write(const char *path) const;
    fs::Error read(const char *path);

    void write(FlashWriter &flashWriter) const;
    bool read(FlashReader &flashReader);

    void writeToFlash() const;
    bool readFromFlash();

private:
    Calibration _calibration;
};
