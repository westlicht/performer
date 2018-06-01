#pragma once

#include "Config.h"

#include "Serialize.h"

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

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        void setPattern(int trackIndex, int pattern) {
            uint32_t patterns = _patterns & ~(0xf << (trackIndex << 2));
            patterns |= (pattern & 0xf) << (trackIndex << 2);
            _patterns = patterns;
        }

        void setPattern(int pattern) {
            uint32_t patterns = pattern & 0xf;
            patterns |= patterns << 4;
            patterns |= patterns << 8;
            patterns |= patterns << 16;
            _patterns = patterns;
        }

        uint32_t _patterns;

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

    //----------------------------------------
    // Methods
    //----------------------------------------

    void chainPattern(int pattern);
    void insertSlot(int slotIndex);
    void removeSlot(int slotIndex);
    void setPattern(int slotIndex, int trackIndex, int pattern);
    void setPattern(int slotIndex, int pattern);
    void swapSlot(int fromIndex, int toIndex);

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    std::array<Slot, CONFIG_SONG_SLOT_COUNT> _slots;
    uint8_t _slotCount;
};
