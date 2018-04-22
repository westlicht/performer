#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Calibration.h"

class CalibrationCvOutputListModel : public ListModel {
public:
    CalibrationCvOutputListModel()
    {}

    void setCvOutput(Calibration::CvOutput &cvOutput) {
        _cvOutput = &cvOutput;
    }

    virtual int rows() const override {
        return Calibration::CvOutput::ItemCount;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            Calibration::CvOutput::itemName(str, row);
        } else if (column == 1) {
            _cvOutput->printItem(row, str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _cvOutput->editItem(row, value, shift);
        }
    }

private:
    Calibration::CvOutput *_cvOutput;
};
