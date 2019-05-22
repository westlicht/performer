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
        // Cache the new track modes because calling setTrackMode will actually
        // trigger a reload of the track setup page and reinitialize the
        // TrackModeListModel leading to only the first track with a new track
        // mode to be updated.
        Track::TrackMode newTrackModes[CONFIG_TRACK_COUNT];
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            newTrackModes[i] = _trackModes[i];
        }
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (newTrackModes[i] != project.track(i).trackMode()) {
                project.setTrackMode(i, newTrackModes[i]);
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
