#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Project.h"

#include <array>

class CvOutputListModel : public ListModel {
public:
    CvOutputListModel(Project &project) :
        _project(project)
    {}

    virtual int rows() const override {
        return CONFIG_TRACK_COUNT;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str("CV%d", row + 1);
        } else if (column == 1) {
            str("Track%d", _project.gateOutput(row) + 1);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _project.editGateOutput(row, value, shift);
        }
    }

private:
    Project &_project;
};
