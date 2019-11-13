#include "Settings.h"

const char *Settings::Filename = "SETTINGS.DAT";

Settings::Settings() {
    clear();
}

void Settings::clear() {
    _calibration.clear();
}

void Settings::write(VersionedSerializedWriter &writer) const {
    _calibration.write(writer);

    writer.writeHash();
}

bool Settings::read(VersionedSerializedReader &reader) {
    clear();

    _calibration.read(reader);

    bool success = reader.checkHash();
    if (!success) {
        clear();
    }

    return success;
}

fs::Error Settings::write(const char *path) const {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    FileHeader header(FileType::Settings, 0, "SETTINGS");
    fileWriter.write(&header, sizeof(header));

    VersionedSerializedWriter writer(
        [&fileWriter] (const void *data, size_t len) { fileWriter.write(data, len); },
        Version
    );

    write(writer);

    return fileWriter.finish();
}

fs::Error Settings::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    VersionedSerializedReader reader(
        [&fileReader] (void *data, size_t len) { fileReader.read(data, len); },
        Version
    );

    bool success = read(reader);

    auto error = fileReader.finish();
    if (error == fs::OK && !success) {
        error = fs::INVALID_CHECKSUM;
    }

    return error;
}

void Settings::write(FlashWriter &flashWriter) const {
    VersionedSerializedWriter writer(
        [&flashWriter] (const void *data, size_t len) { flashWriter.write(data, len); },
        Version
    );

    write(writer);

    flashWriter.finish();
}

bool Settings::read(FlashReader &flashReader) {
    VersionedSerializedReader reader(
        [&flashReader] (void *data, size_t len) { flashReader.read(data, len); },
        Version
    );

    return read(reader);
}

void Settings::writeToFlash() const {
    FlashWriter flashWriter(CONFIG_SETTINGS_FLASH_ADDR, CONFIG_SETTINGS_FLASH_SECTOR);
    write(flashWriter);
}

bool Settings::readFromFlash() {
    FlashReader flashReader(CONFIG_SETTINGS_FLASH_ADDR);
    return read(flashReader);
}
