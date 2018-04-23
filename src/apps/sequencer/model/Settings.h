#pragma once

#include "Calibration.h"
#include "Serialize.h"
#include "FileDefs.h"

class Settings {
public:
    static const char *filename;

    Settings();

    const Calibration &calibration() const { return _calibration; }
          Calibration &calibration()       { return _calibration; }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    fs::Error write(const char *path) const;
    fs::Error read(const char *path);

private:
    Calibration _calibration;
};
