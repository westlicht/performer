#pragma once

#include "ContextMenuPage.h"

#include "ui/model/ContextMenuModel.h"

#include <functional>

class ContextMenu : private ContextMenuModel {
public:
    typedef std::function<void(int)> ActionCallback;
    typedef std::function<bool(int)> IsActiveCallback;

    ContextMenu(
        ContextMenuPage &contextMenuPage,
        const Item items[],
        int itemCount,
        ActionCallback actionCallback,
        IsActiveCallback isActiveCallback = [] (int) { return true; }
    ) :
        _contextMenuPage(contextMenuPage),
        _items(items),
        _itemCount(itemCount),
        _actionCallback(actionCallback),
        _isActiveCallback(isActiveCallback)
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

    ContextMenuPage &_contextMenuPage;
    const Item *_items;
    int _itemCount;
    ActionCallback _actionCallback;
    IsActiveCallback _isActiveCallback;
};
