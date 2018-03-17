#include "TrackSetupPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
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

TrackSetupPage::TrackSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _contextMenu(manager.pages().contextMenu, contextMenuItems, int(ContextAction::Last), [&] (int index) { contextAction(index); })
{}

void TrackSetupPage::enter() {
    _listModel.setTrackSetup(_project.selectedTrackSetup());
}

void TrackSetupPage::exit() {
}

void TrackSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, _listModel.trackSetup() != _project.selectedTrackSetup() ? "COMMIT" : nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void TrackSetupPage::updateLeds(Leds &leds) {
    ListPage::updateLeds(leds);
}

void TrackSetupPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        _contextMenu.show();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    if (key.isFunction() && key.function() == 4) {
        _project.setTrackSetup(_project.selectedTrackIndex(), _listModel.trackSetup());
    }

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        _listModel.setTrackSetup(_project.selectedTrackSetup());
    }

    ListPage::keyPress(event);
}

void TrackSetupPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initTrackSetup();
        break;
    case ContextAction::Copy:
        copyTrackSetup();
        break;
    case ContextAction::Paste:
        pasteTrackSetup();
        break;
    case ContextAction::Last:
        break;
    }
}

void TrackSetupPage::initTrackSetup() {
    _project.setTrackSetup(_project.selectedTrackIndex(), TrackSetup());
    _listModel.setTrackSetup(_project.selectedTrackSetup());
}

void TrackSetupPage::copyTrackSetup() {
    auto &trackSetupBuffer = _model.clipBoard().trackSetupBuffer();

    trackSetupBuffer.copyFrom(_project.selectedTrackSetup());
}

void TrackSetupPage::pasteTrackSetup() {
    auto &trackSetupBuffer = _model.clipBoard().trackSetupBuffer();

    if (trackSetupBuffer.isCopied()) {
        _project.setTrackSetup(_project.selectedTrackIndex(), trackSetupBuffer.trackSetup());
        _listModel.setTrackSetup(_project.selectedTrackSetup());
    }
}

