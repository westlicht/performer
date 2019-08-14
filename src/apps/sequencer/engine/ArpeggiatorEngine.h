#pragma once

#include "SortedQueue.h"

#include "model/Arpeggiator.h"

#include <array>

#include <cstdint>

class ArpeggiatorEngine {
public:
    struct Event {
        enum {
            NoteOn,
            NoteOff,
        } action;
        uint32_t tick;
        uint8_t note;
        uint8_t velocity;
    };

    ArpeggiatorEngine(const Arpeggiator &arpeggiator);

    void reset();

    void noteOn(int note);
    void noteOff(int note);

    void tick(uint32_t tick, int swing);

    bool getEvent(uint32_t tick, Event &event);

private:
    void addNote(int note);
    void removeNote(int note);
    int noteIndexFromOrder(int order);
    void printNotes();

    void advanceStep();
    void advanceOctave();

    static constexpr int MaxNotes = 8;

    const Arpeggiator &_arpeggiator;

    int _stepIndex;
    int _noteIndex;
    uint32_t _noteOrder;
    int8_t _octave;
    int8_t _octaveDirection;

    struct Note {
        uint8_t note;
        uint32_t order;
    };

    std::array<Note, MaxNotes> _notes;
    int8_t _noteCount;
    int8_t _noteHoldCount;

    struct EventCompare {
        bool operator()(const Event &a, const Event &b) {
            return a.tick < b.tick;
        }
    };

    SortedQueue<Event, 16, EventCompare> _eventQueue;
};
