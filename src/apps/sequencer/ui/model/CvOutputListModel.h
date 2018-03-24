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
            int trackIndex = _project.cvOutputTrack(row);
            int outputIndex = 0;
            for (int i = 0; i < row; ++i) {
                outputIndex += _project.cvOutputTrack(i) == trackIndex ? 1 : 0;
            }
            str("Track%d:", trackIndex + 1);
            _project.track(trackIndex).cvOutputName(outputIndex, str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _project.editCvOutputTrack(row, value, shift);
        }
    }

private:
    Project &_project;
};
