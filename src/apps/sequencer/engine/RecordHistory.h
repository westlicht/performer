#pragma once

#include "core/utils/RingBuffer.h"
#include "core/midi/MidiMessage.h"

#include <array>
#include <algorithm>

#include <cstdint>
#include <cinttypes>

class RecordHistory {
public:
    enum class Type : uint8_t {
        NoteOn,
        NoteOff,
    };

    struct Event {
        uint32_t tick;
        Type type;
        int8_t note;
    };

    RecordHistory() {
        clear();
    }

    void clear() {
        _activeNote = -1;
        _size = 0;
        _write = 0;
    }

    size_t size() const { return _size; }

    void write(uint32_t tick, Type type, int note) {
        if (note < 0 || note > 127) {
            return;
        }

        switch (type) {
        case Type::NoteOn:
            if (_activeNote >= 0 && _activeNote != note) {
                write({ tick, Type::NoteOff, _activeNote });
            }
            _activeNote = note;
            write({ tick, Type::NoteOn, int8_t(note) });
            break;
        case Type::NoteOff:
            if (_activeNote == note) {
                _activeNote = -1;
                write({ tick, Type::NoteOff, int8_t(note) });
            }
            break;
        }
    }

    void write(uint32_t tick, const MidiMessage &message) {
        if (message.isNoteOn()) {
            write(tick, Type::NoteOn, message.note());
        } else if (message.isNoteOff()) {
            write(tick, Type::NoteOff, message.note());
        }
    }

    const Event &operator[](int index) const {
        return _events[(_write + index - _size + _events.size()) % _events.size()];
    }

    bool isNoteActive() const {
        return _activeNote >= 0;
    }

    int activeNote() const {
        return _activeNote;
    }

private:
    void write(const Event &event) {
        _events[_write] = event;
        _write = (_write + 1) % _events.size();
        _size = std::min(_events.size(), _size + 1);
    }

    int8_t _activeNote;
    size_t _size;
    size_t _write;
    std::array<Event, 4> _events;
};
