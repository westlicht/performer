#include "SequenceBuilder.h"

class NoteSequenceBuilder : public SequenceBuilder {
public:
    NoteSequenceBuilder(NoteSequence &noteSequence) :
        _noteSequence(noteSequence),
        _originalSequence(noteSequence)
    {
    }

    void clear() override {
        _noteSequence.clear();
    }

    void revert() override {
        _noteSequence = _originalSequence;
    }

    int length() const override {
        return _originalSequence.lastStep() - _originalSequence.firstStep() + 1;
    }

    bool gate(int index) const override {
        int stepIndex = _originalSequence.firstStep() + index;
        return _originalSequence.step(stepIndex).gate();
    }

    float value(int index) const override {
        int stepIndex = _originalSequence.firstStep() + index;
        return _originalSequence.step(stepIndex).note();
    }

    void setLength(int length) override {
        _noteSequence.setFirstStep(0);
        _noteSequence.setLastStep(length - 1);
    }

    void setGate(int index, bool gate) override {
        int stepIndex = _noteSequence.firstStep() + index;
        _noteSequence.step(stepIndex).setGate(gate);
    }

    void setValue(int index, float value) override {
        int stepIndex = _noteSequence.firstStep() + index;
        _noteSequence.step(stepIndex).setNote(value);
    }

private:
    NoteSequence &_noteSequence;
    NoteSequence _originalSequence;
};
