#include "Song.h"

void Song::Slot::clear() {
    _patterns = 0;
}

void Song::Slot::write(WriteContext &context) const {
    auto &writer = context.writer;

    writer.write(_patterns);
}

void Song::Slot::read(ReadContext &context) {
    auto &reader = context.reader;

    reader.read(_patterns);
}

void Song::chainPattern(int pattern) {
    if (!isFull()) {
        slot(_slotCount).setPattern(pattern);
        ++_slotCount;
    }
}

void Song::insertSlot(int slotIndex) {
    if (!isFull() && slotIndex >= 0 && slotIndex < int(_slots.size())) {
        for (int i = int(_slots.size()) - 1; i > slotIndex; --i) {
            slot(i) = slot(i - 1);
        }
        slot(slotIndex).clear();
        ++_slotCount;
    }
}

void Song::removeSlot(int slotIndex) {
    if (slotIndex >= 0 && slotIndex < _slotCount) {
        for (int i = slotIndex; i < int(_slots.size()) - 1; ++i) {
            slot(i) = slot(i + 1);
        }
        slot(_slots.size() - 1).clear();
        --_slotCount;
    }
}

void Song::setPattern(int slotIndex, int trackIndex, int pattern) {
    if (slotIndex >= 0 && slotIndex < _slotCount) {
        slot(slotIndex).setPattern(trackIndex, pattern);
    }
}

void Song::setPattern(int slotIndex, int pattern) {
    if (slotIndex >= 0 && slotIndex < _slotCount) {
        slot(slotIndex).setPattern(pattern);
    }
}

void Song::swapSlot(int fromIndex, int toIndex) {
    if (fromIndex >= 0 && fromIndex < _slotCount && toIndex >= 0 && toIndex < _slotCount) {
        std::swap(slot(fromIndex), slot(toIndex));
    }
}

void Song::clear() {
    for (auto &slot : _slots) {
        slot.clear();
    }
    _slotCount = 0;
}

void Song::write(WriteContext &context) const {
    auto &writer = context.writer;

    writeArray(context, _slots);

    writer.write(_slotCount);
}

void Song::read(ReadContext &context) {
    auto &reader = context.reader;

    readArray(context, _slots);

    reader.read(_slotCount);
}
