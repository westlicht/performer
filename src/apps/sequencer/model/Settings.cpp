#include "Settings.h"
#include "FlashWriter.h"
#include "FlashReader.h"

const char *Settings::Filename = "SETTINGS.DAT";

Settings::Settings() {
    clear();
}

void Settings::clear() {
    _calibration.clear();
    _userSettings.clear();
}

void Settings::write(VersionedSerializedWriter &writer) const {
    _calibration.write(writer);
    _userSettings.write(writer);

    writer.writeHash();
}

bool Settings::read(VersionedSerializedReader &reader) {
    clear();

    _calibration.read(reader);
    _userSettings.read(reader);

    bool success = reader.checkHash();
    if (!success) {
        clear();
    }

    return success;
}

void Settings::writeToFlash() const {
    FlashWriter flashWriter(CONFIG_SETTINGS_FLASH_ADDR, CONFIG_SETTINGS_FLASH_SECTOR);

    VersionedSerializedWriter writer(
        [&flashWriter] (const void *data, size_t len) { flashWriter.write(data, len); },
        Version
    );

    write(writer);

    flashWriter.finish();
}

bool Settings::readFromFlash() {
    FlashReader flashReader(CONFIG_SETTINGS_FLASH_ADDR);

    VersionedSerializedReader reader(
        [&flashReader] (void *data, size_t len) { flashReader.read(data, len); },
        Version
    );

    return read(reader);
}
