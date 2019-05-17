#include "ArpeggiatorEngine.h"

#include "Config.h"

#include "core/Debug.h"
#include "core/utils/Random.h"

#include "os/os.h"

#include <cinttypes>

static Random rng;

ArpeggiatorEngine::ArpeggiatorEngine(const Arpeggiator &arpeggiator) :
    _arpeggiator(arpeggiator)
{
    reset();
}

void ArpeggiatorEngine::reset() {
    _stepIndex = -1;
    _octaveIndex = -1;
    _noteIndex = 0;
    _noteOrder = 0;

    _noteCount = 0;
    _noteHoldCount = 0;
}

void ArpeggiatorEngine::noteOn(int note) {
    if (_arpeggiator.hold() && _noteHoldCount == 0) {
        reset();
    }
    addNote(note);
    // printNotes();
}

void ArpeggiatorEngine::noteOff(int note) {
    removeNote(note);
    // printNotes();
}

void ArpeggiatorEngine::tick(uint32_t tick) {
    if (!_arpeggiator.hold() && _noteHoldCount == 0) {
        reset();
    }

    uint32_t divisor = _arpeggiator.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);

    if (tick % divisor == 0) {
        if (_noteCount > 0) {
            advanceStep();
            if (_stepIndex == 0) {
                _octaveIndex = (_octaveIndex + 1) % _arpeggiator.octaves();
            }
            uint32_t length = std::max(uint32_t(1), uint32_t((divisor * _arpeggiator.gateLength()) / 100));
            uint8_t note = uint8_t(clamp(_notes[_noteIndex].note + _octaveIndex * 12, 0, 127));
            _eventQueue.push({ Event::NoteOn, tick, note, 127 });
            _eventQueue.push({ Event::NoteOff, tick + length, note, 0 });
        }
    }
}

bool ArpeggiatorEngine::getEvent(uint32_t tick, Event &event) {
    if (!_eventQueue.empty() && tick >= _eventQueue.front().tick) {
        event = _eventQueue.front();
        _eventQueue.pop();
        return true;
    }
    return false;
}

void ArpeggiatorEngine::addNote(int note) {
    // exit if note set is full
    if (_noteCount >= MaxNotes) {
        return;
    }

    // find insert position
    int pos = 0;
    while (pos < _noteCount && note > _notes[pos].note) {
        ++pos;
    }

    // exit if note is already in note set
    if (pos < _noteCount && note == _notes[pos].note) {
        return;
    }

    // insert into ordered note set
    ++_noteCount;
    ++_noteHoldCount;
    for (int i = _noteCount - 1; i > pos; --i) {
        _notes[i] = _notes[i - 1];
    }
    _notes[pos].note = note;
    _notes[pos].order = _noteOrder++;
}

void ArpeggiatorEngine::removeNote(int note) {
    for (int i = 0; i < _noteCount; ++i) {
        if (note == _notes[i].note) {
            _noteHoldCount = _noteHoldCount > 0 ? _noteHoldCount - 1 : 0;
            // do not remove note in hold mode
            if (_arpeggiator.hold()) {
                return;
            }
            --_noteCount;
            for (int j = i; j < _noteCount; ++j) {
                _notes[j] = _notes[j + 1];
            }
            return;
        }
    }
}

int ArpeggiatorEngine::noteIndexFromOrder(int order) {
    // search note index of note with given relative order
    for (int noteIndex = 0; noteIndex < _noteCount; ++noteIndex) {
        int currentOrder = 0;
        for (int i = 0; i < _noteCount; ++i) {
            if (_notes[i].order < _notes[noteIndex].order) {
                ++currentOrder;
            }
        }
        if (currentOrder == order) {
            return noteIndex;
        }
    }
    return 0;
}

void ArpeggiatorEngine::printNotes() {
    DBG("notes:");
    for (int i = 0; i < _noteCount; ++i) {
        DBG("note: %" PRIu8 ", order: %" PRIu32, _notes[i].note, _notes[i].order);
    }
}

void ArpeggiatorEngine::advanceStep() {
    _noteIndex = 0;

    auto mode = _arpeggiator.mode();

    switch (mode) {
    case Arpeggiator::Mode::PlayOrder:
        _stepIndex = (_stepIndex + 1) % _noteCount;
        _noteIndex = noteIndexFromOrder(_stepIndex);
        break;
    case Arpeggiator::Mode::Up:
    case Arpeggiator::Mode::Down:
        _stepIndex = (_stepIndex + 1) % _noteCount;
        _noteIndex = _stepIndex;
        break;
    case Arpeggiator::Mode::UpDown:
    case Arpeggiator::Mode::DownUp:
        if (_noteCount >= 2) {
            _stepIndex = (_stepIndex + 1) % ((_noteCount - 1) * 2);
            _noteIndex = _stepIndex % (_noteCount - 1);
            _noteIndex = _stepIndex < _noteCount - 1 ? _noteIndex : _noteCount - _noteIndex - 1;
        } else {
            _stepIndex = 0;
        }
        break;
    case Arpeggiator::Mode::UpAndDown:
    case Arpeggiator::Mode::DownAndUp:
        _stepIndex = (_stepIndex + 1) % (_noteCount * 2);
        _noteIndex = _stepIndex % _noteCount;
        _noteIndex = _stepIndex < _noteCount ? _noteIndex : _noteCount - _noteIndex - 1;
        break;
    case Arpeggiator::Mode::Converge:
        _stepIndex = (_stepIndex + 1) % _noteCount;
        _noteIndex = _stepIndex / 2;
        if (_stepIndex % 2 == 1) {
            _noteIndex = _noteCount - _noteIndex - 1;
        }
        break;
    case Arpeggiator::Mode::Diverge:
        _stepIndex = (_stepIndex + 1) % _noteCount;
        _noteIndex = _stepIndex / 2;
        _noteIndex = _noteCount / 2 + ((_stepIndex % 2 == 0) ? _noteIndex : - _noteIndex - 1);
        break;
    case Arpeggiator::Mode::Random:
        _stepIndex = (_stepIndex + 1) % _noteCount;
        _noteIndex = rng.nextRange(_noteCount);
        break;
    case Arpeggiator::Mode::Last:
        break;
    }

    switch (mode) {
    case Arpeggiator::Mode::Down:
    case Arpeggiator::Mode::DownUp:
    case Arpeggiator::Mode::DownAndUp:
        _noteIndex = _noteCount - _noteIndex - 1;
        break;
    default:
        break;
    }
}
