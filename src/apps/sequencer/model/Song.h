#pragma once

#include "Config.h"

#include "Serialize.h"

#include "core/math/Math.h"

#include <array>

#include <cstdint>

class Song {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    class Slot {
    public:
        int pattern(int trackIndex) const {
            return (_patterns >> (trackIndex << 2)) & 0xf;
        }

        int repeats() const { return _repeats; }

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        static uint32_t fillPatterns(int pattern) {
            uint32_t patterns = pattern & 0xf;
            patterns |= patterns << 4;
            patterns |= patterns << 8;
            patterns |= patterns << 16;
            return patterns;
        }

        void setPattern(int trackIndex, int pattern) {
            pattern = clamp(pattern, 0, CONFIG_PATTERN_COUNT - 1);
            uint32_t patterns = _patterns & ~(0xf << (trackIndex << 2));
            patterns |= (pattern & 0xf) << (trackIndex << 2);
            _patterns = patterns;
        }

        void setPattern(int pattern) {
            _patterns = fillPatterns(pattern);
        }

        void setRepeats(int repeats) {
            _repeats = clamp(repeats, 1, 128);
        }

        uint32_t _patterns;
        uint8_t _repeats;

        friend class Song;
    };

    //----------------------------------------
    // Properties
    //----------------------------------------

    // slots

    const Slot &slot(int slot) const { return _slots[slot]; }
          Slot &slot(int slot)       { return _slots[slot]; }

    int slotCount() const { return _slotCount; }
    bool isFull() const { return _slotCount >= _slots.size(); }
    bool isActiveSlot(int slotIndex) const { return slotIndex >= 0 && slotIndex < _slotCount; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void chainPattern(int pattern);
    void insertSlot(int slotIndex);
    void removeSlot(int slotIndex);
    void duplicateSlot(int slotIndex);
    void swapSlot(int fromIndex, int toIndex);

    void setPattern(int slotIndex, int pattern);
    void setPattern(int slotIndex, int trackIndex, int pattern);
    void editPattern(int slotIndex, int trackIndex, int value);
    void setRepeats(int slotIndex, int repeats);
    void editRepeats(int slotIndex, int value);

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    std::array<Slot, CONFIG_SONG_SLOT_COUNT> _slots;
    uint8_t _slotCount;
};
