#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/FileManager.h"

class FileSelectListModel : public ListModel {
public:
    void setType(FileType type) {
        _type = type;
    }

    virtual int rows() const override {
        return 128;
    }

    virtual int columns() const override {
        return 1;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(row, str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
    }

private:
    void formatName(int row, StringBuilder &str) const {
        FileManager::SlotInfo info;
        FileManager::slotInfo(_type, row, info);
        str("%d: %s", row + 1, info.used ? info.name : "(empty)");
    }

    FileType _type;
};
