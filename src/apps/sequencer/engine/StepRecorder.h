#pragma once

#include "model/NoteSequence.h"
#include "core/midi/MidiMessage.h"

#include <functional>

class StepRecorder {
public:
    void start(const NoteSequence &sequence) {
        _stepIndex = sequence.firstStep();
        _pressedStepIndex = -1;
        _pressedNote = -1;
    }

    void stop() {
        _stepIndex = -1;
    }

    bool enabled() {
        return _stepIndex != -1;
    }

    int stepIndex() const {
        return _stepIndex;
    }

    void setStepIndex(int stepIndex) {
        _stepIndex = stepIndex;
    }

    void process(const MidiMessage &message, NoteSequence &sequence, std::function<int(int)> noteFromMidiNote) {
        if (message.isNoteOn()) {
            // record to step
            auto &step = sequence.step(_stepIndex);
            step.setGate(true);
            step.setLength(NoteSequence::Length::Max / 2);
            step.setNote(noteFromMidiNote(message.note()));

            // remember last edited step
            _pressedNote = message.note();
            _pressedStepIndex = _stepIndex;
        } else if (message.isNoteOff()) {
            if (message.note() == _pressedNote) {
                _pressedNote = -1;
                _pressedStepIndex = -1;

                // move to next step
                ++_stepIndex;
                if (_stepIndex > sequence.lastStep()) {
                    _stepIndex = sequence.firstStep();
                }
            }
        } else if (message.isPitchBend()) {
            // tag slide
            if (_pressedStepIndex != -1) {
                sequence.step(_pressedStepIndex).setSlide(true);
            }
        } else if (message.isControlChange() && message.controlNumber() == 1) {
            // tag tie
            if (_pressedStepIndex != -1) {
                sequence.step(_pressedStepIndex).setLength(NoteSequence::Length::Max);
            }
        }
    }

private:
    int8_t _stepIndex = -1;
    int8_t _pressedStepIndex = -1;
    int8_t _pressedNote = -1;
};
