#include "Settings.h"

const char *Settings::Filename = "SETTINGS.DAT";

Settings::Settings() {
    clear();
}

void Settings::clear() {
    _calibration.clear();
}

void Settings::write(WriteContext &context) const {
    _calibration.write(context);

    context.writer.writeHash();
}

bool Settings::read(ReadContext &context) {
    clear();

    _calibration.read(context);

    bool success = context.reader.checkHash();
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

    WriteContext context = { writer };
    write(context);

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

    ReadContext context = { reader };
    bool success = read(context);

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

    WriteContext context = { writer };
    write(context);

    flashWriter.finish();
}

bool Settings::read(FlashReader &flashReader) {
    VersionedSerializedReader reader(
        [&flashReader] (void *data, size_t len) { flashReader.read(data, len); },
        Version
    );

    ReadContext context = { reader };
    return read(context);
}

void Settings::writeToFlash() const {
    FlashWriter flashWriter(CONFIG_SETTINGS_FLASH_ADDR, CONFIG_SETTINGS_FLASH_SECTOR);
    write(flashWriter);
}

bool Settings::readFromFlash() {
    FlashReader flashReader(CONFIG_SETTINGS_FLASH_ADDR);
    return read(flashReader);
}
