#pragma once

#include "Config.h"

#include "ListModel.h"

#include "ui/utils/AdjustUtils.h"

#include "model/Project.h"

class ProjectListModel : public ListModel {
public:
    ProjectListModel(Project &project) :
        _project(project)
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
        Name,
        BPM,
        Swing,
        GlobalMeasure,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Name:              return "Name";
        case BPM:               return "BPM";
        case Swing:             return "Swing";
        case GlobalMeasure:     return "Global Measure";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Name:
            str(_project.name());
            break;
        case BPM:
            str("%.1f", _project.bpm());
            break;
        case Swing:
            str("%d%%", _project.swing());
            break;
        case GlobalMeasure:
            str("%d", _project.globalMeasure());
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value) {
        switch (item) {
        case Name:
            // _trackSetup.setMode(adjustedEnum(_trackSetup.mode(), value));
            break;
        case BPM:
            _project.setBpm(_project.bpm() + 0.1f * value);
            break;
        case Swing:
            _project.setSwing(_project.swing() + value);
            break;
        case GlobalMeasure:
            _project.setGlobalMeasure(_project.globalMeasure() + value);
            break;
        case Last:
            break;
        }
    }

    Project &_project;
};
