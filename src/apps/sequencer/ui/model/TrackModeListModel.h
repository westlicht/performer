#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Project.h"

#include <array>

class TrackModeListModel : public ListModel {
public:
    TrackModeListModel(Project &project) {
        fromProject(project);
    }

    bool sameAsProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (_trackModes[i] != project.track(i).trackMode()) {
                return false;
            }
        }
        return true;
    }

    void fromProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            _trackModes[i] = project.track(i).trackMode();
        }
    }

    void toProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (_trackModes[i] != project.track(i).trackMode()) {
                project.setTrackMode(i, _trackModes[i]);
            }
        }
    }

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
            str(Track::trackModeName(_trackModes[row]));
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _trackModes[row] = ModelUtils::adjustedEnum(_trackModes[row], value);
        }
    }

private:
    std::array<Track::TrackMode, CONFIG_TRACK_COUNT> _trackModes;
};
