#include "StageSequencePage.h"

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

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
};


StageSequencePage::StageSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void StageSequencePage::enter() {
    _listModel.setSequence(&_project.selectedStageSequence());
}

void StageSequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void StageSequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void StageSequencePage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void StageSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
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

void StageSequencePage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void StageSequencePage::contextAction(int index) {
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

bool StageSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteStageSequence();
    default:
        return true;
    }
}

void StageSequencePage::initSequence() {
    _project.selectedStageSequence().clear();
    showMessage("SEQUENCE INITIALIZED");
}

void StageSequencePage::copySequence() {
    _model.clipBoard().copyStageSequence(_project.selectedStageSequence());
    showMessage("SEQUENCE COPIED");
}

void StageSequencePage::pasteSequence() {
    _model.clipBoard().pasteStageSequence(_project.selectedStageSequence());
    showMessage("SEQUENCE PASTED");
}

void StageSequencePage::duplicateSequence() {
    // TODO
    // _project.selectedStageSequence().duplicate();
    showMessage("SEQUENCE DUPLICATED");
}
