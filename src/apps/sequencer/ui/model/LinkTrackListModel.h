#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Project.h"

#include <array>

class LinkTrackListModel : public ListModel {
public:
    LinkTrackListModel(Project &project) :
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
            str("Track%d", row + 1);
        } else if (column == 1) {
            _project.track(row).printLinkTrack(str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _project.track(row).editLinkTrack(value, shift);
        }
    }

private:
    Project &_project;
};
