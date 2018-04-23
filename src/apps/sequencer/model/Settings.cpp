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

    Writer writer(fileWriter);
    FileHeader header(FileType::Settings, 0, "SETTINGS");
    writer.write(&header, sizeof(header));

    WriteContext context = { writer };
    write(context);

    return fileWriter.finish();
}

fs::Error Settings::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        fileReader.error();
    }

    Reader reader(fileReader);

    FileHeader header;
    reader.read(&header, sizeof(header));

    ReadContext context = { reader };
    read(context);

    return fileReader.finish();
}
