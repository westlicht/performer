#include "UserScalePage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
};

static const char *functionNames[] = { "USER1", "USER2", "USER3", "USER4", nullptr };

UserScalePage::UserScalePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{
    setSelectedIndex(0);
}

void UserScalePage::enter() {
}

void UserScalePage::exit() {
}

void UserScalePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "USER SCALE");
    FixedStringBuilder<8> str("USER%d", _selectedIndex + 1);
    WindowPainter::drawActiveFunction(canvas, str);
    WindowPainter::drawFooter(canvas, functionNames, _keyState);;

    ListPage::draw(canvas);
}

void UserScalePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isFunction()) {
        if (key.function() < CONFIG_USER_SCALE_COUNT) {
            setSelectedIndex(key.function());
        }
        event.consume();
    }

    // if (key.isEncoder()) {
    //     _manager.pages().route.show(selectedRow());
    //     event.consume();
    //     return;
    // }

    ListPage::keyPress(event);
}

void UserScalePage::setSelectedIndex(int index) {
    _selectedIndex = index;
    _userScale = &UserScale::userScales[index];
    _listModel.setUserScale(*_userScale);
}

void UserScalePage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void UserScalePage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initUserScale();
        break;
    case ContextAction::Copy:
        copyUserScale();
        break;
    case ContextAction::Paste:
        pasteUserScale();
        break;
    case ContextAction::Last:
        break;
    }
}

bool UserScalePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteUserScale();
    default:
        return true;
    }
}

void UserScalePage::initUserScale() {
    _userScale->clear();
    showMessage("USER SCALE INITIALIZED");
}

void UserScalePage::copyUserScale() {
    _model.clipBoard().copyUserScale(*_userScale);
    showMessage("USER SCALE COPIED");
}

void UserScalePage::pasteUserScale() {
    _model.clipBoard().pasteUserScale(*_userScale);
    showMessage("USER SCALE PASTED");
}
