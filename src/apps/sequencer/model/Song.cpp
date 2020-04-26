#include "Song.h"

#include "ProjectVersion.h"

// Song::Slot

void Song::Slot::clear() {
    _patterns = 0;
    _mutes = 0;
    _repeats = 1;
}

void Song::Slot::write(VersionedSerializedWriter &writer) const {
    writer.write(_patterns);
    writer.write(_mutes);
    writer.write(_repeats);
}

void Song::Slot::read(VersionedSerializedReader &reader) {
    reader.read(_patterns);
    reader.read(_mutes, ProjectVersion::Version25);
    reader.read(_repeats);
}

// Song

void Song::chainPattern(int pattern) {
    if (!isFull()) {
        if (_slotCount > 0 && slot(_slotCount - 1)._patterns == Slot::fillPatterns(pattern)) {
            editRepeats(_slotCount - 1, 1);
        } else {
            slot(_slotCount).clear();
            slot(_slotCount).setPattern(pattern);
            ++_slotCount;
        }
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
    if (isActiveSlot(slotIndex)) {
        for (int i = slotIndex; i < int(_slots.size()) - 1; ++i) {
            slot(i) = slot(i + 1);
        }
        slot(_slots.size() - 1).clear();
        --_slotCount;
    }
}

void Song::duplicateSlot(int slotIndex) {
    if (!isFull() && slotIndex >= 0 && slotIndex < int(_slots.size()) && _slotCount < int(_slots.size())) {
        insertSlot(slotIndex + 1);
        _slots[slotIndex + 1] = _slots[slotIndex];
    }
}

void Song::swapSlot(int fromIndex, int toIndex) {
    if (fromIndex >= 0 && fromIndex < _slotCount && toIndex >= 0 && toIndex < _slotCount) {
        std::swap(slot(fromIndex), slot(toIndex));
    }
}

void Song::setPattern(int slotIndex, int pattern) {
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).setPattern(pattern);
    }
}

void Song::setPattern(int slotIndex, int trackIndex, int pattern) {
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).setPattern(trackIndex, pattern);
    }
}

void Song::editPattern(int slotIndex, int trackIndex, int value) {
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).setPattern(trackIndex, slot(slotIndex).pattern(trackIndex) + value);
    }
}

void Song::setMute(int slotIndex, int trackIndex, bool mute) {
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).setMute(trackIndex, mute);
    }
}

void Song::toggleMute(int slotIndex, int trackIndex)
{
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).toggleMute(trackIndex);
    }
}

void Song::setRepeats(int slotIndex, int repeats) {
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).setRepeats(repeats);
    }
}

void Song::editRepeats(int slotIndex, int value) {
    if (isActiveSlot(slotIndex)) {
        slot(slotIndex).setRepeats(slot(slotIndex).repeats() + value);
    }
}

bool Song::trackHasMutes(int trackIndex) const {
    for (int i = 0; i < _slotCount; ++i) {
        if (slot(i).mute(trackIndex)) return true;
    }
    return false;
}

void Song::clear() {
    for (auto &slot : _slots) {
        slot.clear();
    }
    _slotCount = 0;
}

void Song::write(VersionedSerializedWriter &writer) const {
    writeArray(writer, _slots);

    writer.write(_slotCount);
}

void Song::read(VersionedSerializedReader &reader) {
    if (reader.dataVersion() < ProjectVersion::Version18) {
        readArray(reader, _slots, 16);
    } else {
        readArray(reader, _slots);
    }

    reader.read(_slotCount);
}
