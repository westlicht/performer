#include "NoteSequencePage.h"

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


NoteSequencePage::NoteSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _contextMenu(
        manager.pages().contextMenu,
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    )
{}

void NoteSequencePage::enter() {
    _listModel.setSequence(&_project.selectedNoteSequence());
}

void NoteSequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void NoteSequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void NoteSequencePage::updateLeds(Leds &leds) {
    const auto &sequence = _project.selectedNoteSequence();

    switch (selectedRow()) {
    case NoteSequenceListModel::Scale:
        LedPainter::drawStepIndex(leds, sequence.scale());
        break;
    case NoteSequenceListModel::Divisor:
        LedPainter::drawStepIndex(leds, sequence.indexedDivisor());
        break;
    case NoteSequenceListModel::FirstStep:
        LedPainter::drawStepIndex(leds, sequence.firstStep());
        break;
    case NoteSequenceListModel::LastStep:
        LedPainter::drawStepIndex(leds, sequence.lastStep());
        break;
    }
}

void NoteSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedNoteSequence();

    if (key.isContextMenu()) {
        _contextMenu.show();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    if (key.isStep()) {
        int step = key.step();
        switch (selectedRow()) {
        case NoteSequenceListModel::Scale:
            sequence.setScale(step);
            break;
        case NoteSequenceListModel::Divisor:
            sequence.setIndexedDivisor(step);
            break;
        }
        event.consume();
    }

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
}

void NoteSequencePage::contextAction(int index) {
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

bool NoteSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteNoteSequence();
    default:
        return true;
    }
}

void NoteSequencePage::initSequence() {
    _project.selectedNoteSequence().clear();
    showMessage("SEQUENCE INITIALIZED");
}

void NoteSequencePage::copySequence() {
    _model.clipBoard().copyNoteSequence(_project.selectedNoteSequence());
    showMessage("SEQUENCE COPIED");
}

void NoteSequencePage::pasteSequence() {
    _model.clipBoard().pasteNoteSequence(_project.selectedNoteSequence());
    showMessage("SEQUENCE PASTED");
}

void NoteSequencePage::duplicateSequence() {
    // _project.selectedNoteSequence().duplicate();
    showMessage("SEQUENCE DUPLICATED");
}
