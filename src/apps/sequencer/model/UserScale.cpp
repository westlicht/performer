#include "UserScale.h"
#include "Project.h"

UserScale::Array UserScale::userScales;

UserScale::UserScale() :
    Scale("")
{
    clear();
}

void UserScale::clear() {
    StringUtils::copy(_name, "INIT", sizeof(_name));
    setMode(Mode::Chromatic);
    clearItems();
}

void UserScale::clearItems() {
    setSize(1);
    _items.fill(0);
    if (_mode == Mode::Voltage) {
        _items[1] = 1000;
    }
}

void UserScale::write(WriteContext &context) const {
    auto &writer = context.writer;

    writer.write(_name, NameLength + 1);
    writer.write(_mode);
    writer.write(_size);

    for (int i = 0; i < _size; ++i) {
        writer.write(_items[i]);
    }

    writer.writeHash();
}

bool UserScale::read(ReadContext &context) {
    clear();

    auto &reader = context.reader;

    reader.read(_name, NameLength + 1, Project::Version5);
    reader.read(_mode);
    reader.read(_size);

    for (int i = 0; i < _size; ++i) {
        reader.read(_items[i]);
    }

    bool success = reader.checkHash();
    if (!success) {
        clear();
    }

    return success;
}

fs::Error UserScale::write(const char *path) const {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    FileHeader header(FileType::UserScale, 0, _name);
    fileWriter.write(&header, sizeof(header));

    VersionedSerializedWriter writer(
        [&fileWriter] (const void *data, size_t len) { fileWriter.write(data, len); },
        Project::Version
    );

    WriteContext context = { writer };
    write(context);

    return fileWriter.finish();
}

fs::Error UserScale::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    VersionedSerializedReader reader(
        [&fileReader] (void *data, size_t len) { fileReader.read(data, len); },
        Project::Version
    );

    ReadContext context = { reader };
    bool success = read(context);

    // TODO at some point we should remove this because name is also stored with data as of version 5
    if (success) {
        header.readName(_name, sizeof(_name));
    }

    auto error = fileReader.finish();
    if (error == fs::OK && !success) {
        error = fs::INVALID_CHECKSUM;
    }

    return error;
}
