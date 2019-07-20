#include "HarmonySequencePage.h"

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


HarmonySequencePage::HarmonySequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void HarmonySequencePage::enter() {
    _listModel.setSequence(&_project.selectedHarmonySequence());
}

void HarmonySequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void HarmonySequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, Track::trackModeName(_project.selectedTrack().trackMode()));
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void HarmonySequencePage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void HarmonySequencePage::keyPress(KeyPressEvent &event) {
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

void HarmonySequencePage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void HarmonySequencePage::contextAction(int index) {
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

bool HarmonySequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteHarmonySequence();
    default:
        return true;
    }
}

void HarmonySequencePage::initSequence() {
    _project.selectedHarmonySequence().clear();
    showMessage("SEQUENCE INITIALIZED");
}

void HarmonySequencePage::copySequence() {
    _model.clipBoard().copyHarmonySequence(_project.selectedHarmonySequence());
    showMessage("SEQUENCE COPIED");
}

void HarmonySequencePage::pasteSequence() {
    _model.clipBoard().pasteHarmonySequence(_project.selectedHarmonySequence());
    showMessage("SEQUENCE PASTED");
}

void HarmonySequencePage::duplicateSequence() {
    // TODO
    // _project.selectedHarmonySequence().duplicate();
    showMessage("SEQUENCE DUPLICATED");
}
