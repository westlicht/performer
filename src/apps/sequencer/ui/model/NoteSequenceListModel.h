#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/NoteSequence.h"
#include "model/Scale.h"

class NoteSequenceListModel : public RoutableListModel {
public:
    enum Item {
        FirstStep,
        LastStep,
        RunMode,
        Divisor,
        ResetMeasure,
        Scale,
        RootNote,
        Last
    };

    NoteSequenceListModel()
    {}

    void setSequence(NoteSequence *sequence) {
        _sequence = sequence;
    }

    virtual int rows() const override {
        return _sequence ? Last : 0;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

    virtual int indexedCount(int row) const override {
        return indexedCountValue(Item(row));
    }

    virtual int indexed(int row) const override {
        return indexedValue(Item(row));
    }

    virtual void setIndexed(int row, int index) override {
        if (index >= 0 && index < indexedCount(row)) {
            setIndexedValue(Item(row), index);
        }
    }

    virtual Routing::Target routingTarget(int row) const override {
        switch (Item(row)) {
        case Divisor:
            return Routing::Target::Divisor;
        case FirstStep:
            return Routing::Target::FirstStep;
        case LastStep:
            return Routing::Target::LastStep;
        case RunMode:
            return Routing::Target::RunMode;
        case Scale:
            return Routing::Target::Scale;
        case RootNote:
            return Routing::Target::RootNote;
        default:
            return Routing::Target::None;
        }
    }

private:
    static const char *itemName(Item item) {
        switch (item) {
        case FirstStep:         return "First Step";
        case LastStep:          return "Last Step";
        case RunMode:           return "Run Mode";
        case Divisor:           return "Divisor";
        case ResetMeasure:      return "Reset Measure";
        case Scale:             return "Scale";
        case RootNote:          return "Root Note";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case FirstStep:
            _sequence->printFirstStep(str);
            break;
        case LastStep:
            _sequence->printLastStep(str);
            break;
        case RunMode:
            _sequence->printRunMode(str);
            break;
        case Divisor:
            _sequence->printDivisor(str);
            break;
        case ResetMeasure:
            _sequence->printResetMeasure(str);
            break;
        case Scale:
            _sequence->printScale(str);
            break;
        case RootNote:
            _sequence->printRootNote(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case FirstStep:
            _sequence->editFirstStep(value, shift);
            break;
        case LastStep:
            _sequence->editLastStep(value, shift);
            break;
        case RunMode:
            _sequence->editRunMode(value, shift);
            break;
        case Divisor:
            _sequence->editDivisor(value, shift);
            break;
        case ResetMeasure:
            _sequence->editResetMeasure(value, shift);
            break;
        case Scale:
            _sequence->editScale(value, shift);
            break;
        case RootNote:
            _sequence->editRootNote(value, shift);
            break;
        case Last:
            break;
        }
    }

    int indexedCountValue(Item item) const {
        switch (item) {
        case FirstStep:
        case LastStep:
            return 16;
        case RunMode:
            return int(Types::RunMode::Last);
        case Divisor:
        case ResetMeasure:
            return 16;
        case Scale:
            return Scale::Count + 1;
        case RootNote:
            return 12 + 1;
        case Last:
            break;
        }
        return -1;
    }

    int indexedValue(Item item) const {
        switch (item) {
        case FirstStep:
            return _sequence->firstStep();
        case LastStep:
            return _sequence->lastStep();
        case RunMode:
            return int(_sequence->runMode());
        case Divisor:
            return _sequence->indexedDivisor();
        case ResetMeasure:
            return _sequence->resetMeasure();
        case Scale:
            return _sequence->indexedScale();
        case RootNote:
            return _sequence->indexedRootNote();
        case Last:
            break;
        }
        return -1;
    }

    void setIndexedValue(Item item, int index) {
        switch (item) {
        case FirstStep:
            return _sequence->setFirstStep(index);
        case LastStep:
            return _sequence->setLastStep(index);
        case RunMode:
            return _sequence->setRunMode(Types::RunMode(index));
        case Divisor:
            return _sequence->setIndexedDivisor(index);
        case ResetMeasure:
            return _sequence->setResetMeasure(index);
        case Scale:
            return _sequence->setIndexedScale(index);
        case RootNote:
            return _sequence->setIndexedRootNote(index);
        case Last:
            break;
        }
    }

    NoteSequence *_sequence;
};
