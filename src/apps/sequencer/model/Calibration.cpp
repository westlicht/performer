#include "Calibration.h"

void Calibration::CvOutput::autoFill() {
    auto fillRange = [this] (int first, int last) {
        int count = last - first;
        for (int i = first + 1; i < last; ++i) {
            _items[i] = (_items[last] - _items[first]) * (i - first) * count;
        }
        DBG("fill %d - %d", first, last);
    };

    int first = -1;
    int last = -1;

    for (size_t i = 0; i < _items.size(); ++i) {
        if (_items[i] != defaultItemValue(i)) {
            if (first == -1) {
                first = i;
            } else if (last == -1) {
                last = i;
                fillRange(first, last);
            } else {
                first = last;
                last = i;
                fillRange(first, last);
            }
        }
    }
}

void Calibration::CvOutput::clear() {
    for (size_t i = 0; i < _items.size(); ++i) {
        _items[i] = defaultItemValue(i);
    }
}

void Calibration::CvOutput::write(WriteContext &context) const {
    auto &writer = context.writer;
    for (size_t i = 0; i < _items.size(); ++i) {
        writer.write(_items[i]);
    }
}

void Calibration::CvOutput::read(ReadContext &context) {
    auto &reader = context.reader;
    for (size_t i = 0; i < _items.size(); ++i) {
        reader.read(_items[i]);
    }
}


void Calibration::clear() {
    for (auto &cvOutput : _cvOutputs) {
        cvOutput.clear();
    }
}

void Calibration::write(WriteContext &context) const {
    for (const auto &cvOutput : _cvOutputs) {
        cvOutput.write(context);
    }
}

void Calibration::read(ReadContext &context) {
    for (auto &cvOutput : _cvOutputs) {
        cvOutput.read(context);
    }
}
