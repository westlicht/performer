#include "Settings.h"

Settings::Settings() {
    clear();
}

void Settings::clear() {
    _calibration.clear();
}

void Settings::write(WriteContext &context) const {
    _calibration.write(context);
}

void Settings::read(ReadContext &context) {
    _calibration.read(context);
}
