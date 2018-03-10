#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/NoteSequence.h"

#include "engine/Scale.h"

class NoteSequenceSetupListModel : public ListModel {
public:
    enum Item {
        Scale,
        Divisor,
        ResetMeasure,
        PlayMode,
        FirstStep,
        LastStep,
        Last
    };

    NoteSequenceSetupListModel()
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

private:
    static const char *itemName(Item item) {
        switch (item) {
        case Scale:             return "Scale";
        case Divisor:           return "Divisor";
        case ResetMeasure:      return "Reset Measure";
        case PlayMode:          return "Play Mode";
        case FirstStep:         return "First Step";
        case LastStep:          return "Last Step";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Scale:
            _sequence->printScale(str);
            break;
        case Divisor:
            _sequence->printDivisor(str);
            break;
        case ResetMeasure:
            _sequence->printResetMeasure(str);
            break;
        case PlayMode:
            _sequence->printPlayMode(str);
            break;
        case FirstStep:
            _sequence->printFirstStep(str);
            break;
        case LastStep:
            _sequence->printLastStep(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Scale:
            _sequence->editScale(value, shift);
            break;
        case Divisor:
            _sequence->editDivisor(value, shift);
            break;
        case ResetMeasure:
            _sequence->editResetMeasure(value, shift);
            break;
        case PlayMode:
            _sequence->editPlayMode(value, shift);
            break;
        case FirstStep:
            _sequence->editFirstStep(value, shift);
            break;
        case LastStep:
            _sequence->editLastStep(value, shift);
            break;
        case Last:
            break;
        }
    }

    NoteSequence *_sequence;
};
