#include "LayoutPage.h"

#include "Pages.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"


LayoutPage::LayoutPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _trackModeListModel),
    _trackModeListModel(context.model.project()),
    _linkTrackListModel(context.model.project()),
    _gateOutputListModel(context.model.project()),
    _cvOutputListModel(context.model.project())
{
}

void LayoutPage::enter() {
    _trackModeListModel.fromProject(_project);
}

void LayoutPage::draw(Canvas &canvas) {
    bool showCommit = _mode == Mode::TrackMode && !_trackModeListModel.sameAsProject(_project);
    const char *functionNames[] = { "MODE", "LINK", "GATE", "CV", showCommit ? "COMMIT" : nullptr };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "LAYOUT");
    WindowPainter::drawActiveFunction(canvas, modeName(_mode));
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), int(_mode));

    ListPage::draw(canvas);
}

void LayoutPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        if (key.function() == 4 && _mode == Mode::TrackMode && !_trackModeListModel.sameAsProject(_project)) {
            _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
                if (result) {
                    setEdit(false);
                    _trackModeListModel.toProject(_project);
                    showMessage("LAYOUT CHANGED");
                }
            });
        }
        setMode(Mode(key.function()));
        event.consume();
    }

    ListPage::keyPress(event);
}

void LayoutPage::setMode(Mode mode) {
    if (mode == _mode) {
        return;
    }
    switch (mode) {
    case Mode::TrackMode:
        setListModel(_trackModeListModel);
        break;
    case Mode::LinkTrack:
        setListModel(_linkTrackListModel);
        break;
    case Mode::GateOutput:
        setListModel(_gateOutputListModel);
        break;
    case Mode::CvOutput:
        setListModel(_cvOutputListModel);
        break;
    default:
        return;
    }
    _mode = mode;
}
