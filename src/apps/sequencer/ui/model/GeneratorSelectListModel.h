#pragma once

#include "Config.h"

#include "ListModel.h"

#include "engine/generators/Generator.h"

class GeneratorSelectListModel : public ListModel {
public:
    virtual int rows() const override {
        return int(Generator::Mode::Last);
    }

    virtual int columns() const override {
        return 1;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str(Generator::modeName(Generator::Mode(row)));
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
    }

};
