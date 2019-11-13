#include "Calibration.h"

void Calibration::CvOutput::clear() {
    for (size_t i = 0; i < _items.size(); ++i) {
        _items[i] = defaultItemValue(i);
    }
}

void Calibration::CvOutput::write(VersionedSerializedWriter &writer) const {
    for (size_t i = 0; i < _items.size(); ++i) {
        writer.write(_items[i]);
    }
}

void Calibration::CvOutput::read(VersionedSerializedReader &reader) {
    for (size_t i = 0; i < _items.size(); ++i) {
        reader.read(_items[i]);
    }
}

void Calibration::CvOutput::update() {
    auto findNextUserDefined = [this] (int index, int direction) {
        while (true) {
            index += direction;
            if (index < 0) {
                return -1;
            }
            if (index >= int(_items.size())) {
                return -1;
            }
            if (userDefined(index)) {
                return index;
            }
        }
    };

    for (int index = 0; index < int(_items.size()); ++index) {
        if (userDefined(index)) {
            continue;
        }

        int prevIndex = findNextUserDefined(index, -1);
        int nextIndex = findNextUserDefined(index,  1);

        if (prevIndex != -1 && nextIndex != -1) {
            float t = float(index - prevIndex) / (nextIndex - prevIndex);
            setItem(index, std::round(lerp(t, float(item(prevIndex)), float(item(nextIndex)))), false);
        } else if (prevIndex != -1) {
            setItem(index, defaultItemValue(index) - defaultItemValue(prevIndex) + item(prevIndex), false);
        } else if (nextIndex != -1) {
            setItem(index, defaultItemValue(index) - defaultItemValue(nextIndex) + item(nextIndex), false);
        } else {
            setItem(index, defaultItemValue(index), false);
        }
    }
}


void Calibration::clear() {
    for (auto &cvOutput : _cvOutputs) {
        cvOutput.clear();
    }
}

void Calibration::write(VersionedSerializedWriter &writer) const {
    for (const auto &cvOutput : _cvOutputs) {
        cvOutput.write(writer);
    }
}

void Calibration::read(VersionedSerializedReader &reader) {
    for (auto &cvOutput : _cvOutputs) {
        cvOutput.read(reader);
    }
}
