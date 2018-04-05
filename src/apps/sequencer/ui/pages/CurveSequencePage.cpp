#include "CurveSequencePage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Duplicate,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
};

CurveSequencePage::CurveSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _contextMenu(
        manager.pages().contextMenu,
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    )
{}

void CurveSequencePage::enter() {
    _listModel.setSequence(&_project.selectedCurveSequence());
}

void CurveSequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void CurveSequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void CurveSequencePage::updateLeds(Leds &leds) {
}

void CurveSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        _contextMenu.show();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
}

void CurveSequencePage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initSequence();
        break;
    case ContextAction::Copy:
        copySequence();
        break;
    case ContextAction::Paste:
        pasteSequence();
        break;
    case ContextAction::Duplicate:
        duplicateSequence();
        break;
    case ContextAction::Last:
        break;
    }
}

bool CurveSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteCurveSequence();
    default:
        return true;
    }
}

void CurveSequencePage::initSequence() {
    _project.selectedCurveSequence().clear();
    showMessage("SEQUENCE INITIALIZED");
}

void CurveSequencePage::copySequence() {
    _model.clipBoard().copyCurveSequence(_project.selectedCurveSequence());
    showMessage("SEQUENCE COPIED");
}

void CurveSequencePage::pasteSequence() {
    _model.clipBoard().pasteCurveSequence(_project.selectedCurveSequence());
    showMessage("SEQUENCE PASTED");
}

void CurveSequencePage::duplicateSequence() {
    // _project.selectedCurveSequence().duplicate();
    showMessage("SEQUENCE DUPLICATED");
}
