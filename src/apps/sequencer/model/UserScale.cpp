#include "UserScale.h"

UserScale::Array UserScale::userScales;

UserScale::UserScale() :
    Scale("User")
{
    clear();
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
