#include "Settings.h"

const char *Settings::filename = "SETTINGS.DAT";

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

fs::Error Settings::write(const char *path) const {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        fileWriter.error();
    }

    FileHeader header(FileType::Settings, 0, "SETTINGS");
    fileWriter.write(&header, sizeof(header));

    Writer writer(fileWriter, Version);
    WriteContext context = { writer };
    write(context);

    return fileWriter.finish();
}

fs::Error Settings::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    Reader reader(fileReader, Version);
    ReadContext context = { reader };
    read(context);

    return fileReader.finish();
}
