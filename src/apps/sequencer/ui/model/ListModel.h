#pragma once

#include "core/utils/StringBuilder.h"

class ListModel {
public:
    virtual int rows() const = 0;
    virtual int columns() const = 0;

    virtual void cell(int row, int column, StringBuilder &str) const = 0;

    virtual void edit(int row, int column, int value, bool shift) = 0;
};
