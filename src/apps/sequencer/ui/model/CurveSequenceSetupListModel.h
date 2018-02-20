#pragma once

#include "Config.h"

#include "ListModel.h"

#include "ui/utils/AdjustUtils.h"

#include "model/CurveSequence.h"

#include "engine/Scale.h"

class CurveSequenceSetupListModel : public ListModel {
public:
    CurveSequenceSetupListModel(CurveSequence &sequence) :
        _sequence(sequence)
    {}

    virtual int rows() const override {
        return Last;
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

    virtual void edit(int row, int column, int value) override {
        if (column == 1) {
            editValue(Item(row), value);
        }
    }

private:
    enum Item {
        // Scale,
        // Divisor,
        // ResetMeasure,
        // PlayMode,
        // FirstStep,
        // LastStep,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        // case Scale:             return "Scale";
        // case Divisor:           return "Divisor";
        // case ResetMeasure:      return "Reset Measure";
        // case PlayMode:          return "Play Mode";
        // case FirstStep:         return "First Step";
        // case LastStep:          return "Last Step";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        // case Scale:
        //     str(Scale::get(_sequence.scale()).name());
        //     break;
        // case Divisor:
        //     str("%d", _sequence.divisor());
        //     break;
        // case ResetMeasure:
        //     str("%d", _sequence.resetMeasure());
        //     break;
        // case PlayMode:
        //     str(NoteSequence::playModeName(_sequence.playMode()));
        //     break;
        // case FirstStep:
        //     str("%d", _sequence.firstStep());
        //     break;
        // case LastStep:
        //     str("%d", _sequence.lastStep());
        //     break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value) {
        switch (item) {
        // case Scale:
        //     _sequence.setScale(_sequence.scale() + value);
        //     break;
        // case Divisor:
        //     _sequence.setDivisor(_sequence.divisor() + value);
        //     break;
        // case ResetMeasure:
        //     _sequence.setResetMeasure(_sequence.resetMeasure() + value);
        //     break;
        // case PlayMode:
        //     _sequence.setPlayMode(adjustedEnum(_sequence.playMode(), value));
        //     break;
        // case FirstStep:
        //     _sequence.setFirstStep(_sequence.firstStep() + value);
        //     break;
        // case LastStep:
        //     _sequence.setLastStep(_sequence.lastStep() + value);
        //     break;
        case Last:
            break;
        }
    }

    CurveSequence &_sequence;
};
