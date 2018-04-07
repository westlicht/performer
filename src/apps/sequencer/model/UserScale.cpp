#include "UserScale.h"

UserScale::Array UserScale::userScales;

UserScale::UserScale() :
    Scale("User")
{
    clear();
}

void UserScale::clear() {
    setMode(Mode::Note);
    setSize(1);
}

void UserScale::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_mode);
}

void UserScale::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_mode);
}
