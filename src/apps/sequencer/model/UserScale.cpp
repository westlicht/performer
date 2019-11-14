#include "UserScale.h"
#include "ProjectVersion.h"

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

void UserScale::write(VersionedSerializedWriter &writer) const {
    writer.write(_name, NameLength + 1);
    writer.write(_mode);
    writer.write(_size);

    for (int i = 0; i < _size; ++i) {
        writer.write(_items[i]);
    }

    writer.writeHash();
}

bool UserScale::read(VersionedSerializedReader &reader) {
    clear();

    reader.read(_name, NameLength + 1, ProjectVersion::Version5);
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
