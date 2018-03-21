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
    _contextMenu(
        manager.pages().contextMenu,
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    )
{}

void TrackSetupPage::enter() {
    _listModel.setTrack(_project.selectedTrack());
}

void TrackSetupPage::exit() {
}

void TrackSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, _listModel.track() != _project.selectedTrack() ? "COMMIT" : nullptr };
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
        _project.setTrackSetup(_project.selectedTrackIndex(), _listModel.track());
    }

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        _listModel.setTrack(_project.selectedTrack());
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

bool TrackSetupPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().trackBuffer().isCopied() && _model.clipBoard().trackBuffer().canPasteTo(_project.selectedTrack());
    default:
        return true;
    }
}

void TrackSetupPage::initTrackSetup() {
    _project.setTrackSetup(_project.selectedTrackIndex(), Track());
    _listModel.setTrack(_project.selectedTrack());
}

void TrackSetupPage::copyTrackSetup() {
    auto &trackBuffer = _model.clipBoard().trackBuffer();

    trackBuffer.copyFrom(_project.selectedTrack());
}

void TrackSetupPage::pasteTrackSetup() {
    auto &trackBuffer = _model.clipBoard().trackBuffer();

    if (trackBuffer.isCopied()) {
        _project.setTrackSetup(_project.selectedTrackIndex(), trackBuffer.track());
        _listModel.setTrack(_project.selectedTrack());
    }
}

