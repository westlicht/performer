#pragma once

#include "ContextMenuPage.h"

#include "ui/model/ContextMenuModel.h"

#include <functional>

class ContextMenu : private ContextMenuModel {
public:
    typedef std::function<void(int)> ActionCallback;
    typedef std::function<bool(int)> ItemEnabledCallback;

    ContextMenu(
        ContextMenuPage &contextMenuPage,
        const Item items[],
        int itemCount,
        ActionCallback actionCallback,
        ItemEnabledCallback itemEnabledCallback = [] (int) { return true; }
    ) :
        _contextMenuPage(contextMenuPage),
        _items(items),
        _itemCount(itemCount),
        _actionCallback(actionCallback),
        _itemEnabledCallback(itemEnabledCallback)
    {
    }

    void show() {
        _contextMenuPage.show(*this, [this] (int index) {
            if (_actionCallback) {
                _actionCallback(index);
            }
        });
    }

private:
    virtual int itemCount() const override {
        return _itemCount;
    }

    virtual const ContextMenuModel::Item &item(int index) const override {
        return _items[index];
    }

    virtual bool itemEnabled(int index) const override {
        return _itemEnabledCallback(index);
    }

    ContextMenuPage &_contextMenuPage;
    const Item *_items;
    int _itemCount;
    ActionCallback _actionCallback;
    ItemEnabledCallback _itemEnabledCallback;
};
