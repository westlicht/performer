#include "UserScale.h"

UserScale::Array UserScale::userScales = {{ "USER1", "USER2", "USER3", "USER4" }};

UserScale::UserScale(const char *name) :
    Scale(name)
{
    clear();
}

UserScale::UserScale() :
    UserScale("User")
{
}

void UserScale::clear() {
    setMode(Mode::Note);
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
    writer.write(_mode);
}

void UserScale::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_mode);
}

fs::Error UserScale::write(const char *path) const {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        fileWriter.error();
    }

    Writer writer(fileWriter);
    WriteContext context = { writer };

    write(context);

    return fileWriter.finish();
}

fs::Error UserScale::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        fileReader.error();
    }

    Reader reader(fileReader);
    ReadContext context = { reader };

    read(context);

    return fileReader.finish();
}
