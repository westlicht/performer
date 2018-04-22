#pragma once

#include "Calibration.h"
#include "Serialize.h"

class Settings {
public:
    Settings();

    const Calibration &calibration() const { return _calibration; }
          Calibration &calibration()       { return _calibration; }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Calibration _calibration;
};
