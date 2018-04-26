#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/CurveSequence.h"

class CurveSequenceListModel : public ListModel {
public:
    enum Item {
        Range,
        Divisor,
        ResetMeasure,
        RunMode,
        FirstStep,
        LastStep,
        Last
    };

    CurveSequenceListModel()
    {}

    void setSequence(CurveSequence *sequence) {
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

    virtual int indexed(int row) const override {
        return indexedValue(Item(row));
    }

    virtual void setIndexed(int row, int index) override {
        setIndexedValue(Item(row), index);
    }

private:
    static const char *itemName(Item item) {
        switch (item) {
        case Range:             return "Range";
        case Divisor:           return "Divisor";
        case ResetMeasure:      return "Reset Measure";
        case RunMode:           return "Run Mode";
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
        case Range:
            _sequence->printRange(str);
            break;
        case Divisor:
            _sequence->printDivisor(str);
            break;
        case ResetMeasure:
            _sequence->printResetMeasure(str);
            break;
        case RunMode:
            _sequence->printRunMode(str);
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
        case Range:
            _sequence->editRange(value, shift);
            break;
        case Divisor:
            _sequence->editDivisor(value, shift);
            break;
        case ResetMeasure:
            _sequence->editResetMeasure(value, shift);
            break;
        case RunMode:
            _sequence->editRunMode(value, shift);
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

    int indexedValue(Item item) const {
        switch (item) {
        case Range:
            return int(_sequence->range());
        case Divisor:
            return _sequence->indexedDivisor();
        case ResetMeasure:
            return _sequence->resetMeasure();
        case RunMode:
            return int(_sequence->runMode());
        case FirstStep:
            return _sequence->firstStep();
        case LastStep:
            return _sequence->lastStep();
        case Last:
            break;
        }
        return -1;
    }

    void setIndexedValue(Item item, int index) {
        switch (item) {
        case Range:
            return _sequence->setRange(Types::VoltageRange(index));
        case Divisor:
            return _sequence->setIndexedDivisor(index);
        case ResetMeasure:
            return _sequence->setResetMeasure(index);
        case RunMode:
            return _sequence->setRunMode(Types::RunMode(index));
        case FirstStep:
            return _sequence->setFirstStep(index);
        case LastStep:
            return _sequence->setLastStep(index);
        case Last:
            break;
        }
    }

    CurveSequence *_sequence = nullptr;
};
