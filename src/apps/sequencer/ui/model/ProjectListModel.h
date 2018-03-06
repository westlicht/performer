#pragma once

#include "Config.h"

#include "ListModel.h"

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

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
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
            _project.printBpm(str);
            break;
        case Swing:
            _project.printSwing(str);
            break;
        case GlobalMeasure:
            _project.printGlobalMeasure(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Name:
            break;
        case BPM:
            _project.editBpm(value, shift);
            break;
        case Swing:
            _project.editSwing(value, shift);
            break;
        case GlobalMeasure:
            _project.editGlobalMeasure(value, shift);
            break;
        case Last:
            break;
        }
    }

    Project &_project;
};
