#pragma once

#include "core/utils/StringBuilder.h"

class ContextMenuModel {
public:
    struct Item {
        const char *title;
        // uint8_t icon;
    };

    virtual int itemCount() const = 0;

    virtual const Item &item(int index) const = 0;
    virtual bool itemEnabled(int index) const = 0;
};
