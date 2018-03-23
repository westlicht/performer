#include "LayoutPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"


LayoutPage::LayoutPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _trackModeListModel),
    _trackModeListModel(context.model.project()),
    _gateOutputListModel(context.model.project()),
    _cvOutputListModel(context.model.project())
{
}

void LayoutPage::enter() {
    _trackModeListModel.fromProject(_project);
}

void LayoutPage::draw(Canvas &canvas) {
    bool showChange = _mode == Mode::TrackMode && !_trackModeListModel.sameAsProject(_project);
    const char *functionNames[] = { "MODE", "GATE", "CV", nullptr, showChange ? "CHANGE" : nullptr };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "LAYOUT");
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);
    WindowPainter::drawActiveFunction(canvas, modeName(_mode));

    ListPage::draw(canvas);
}

void LayoutPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        if (_mode == Mode::TrackMode && key.function() == 4) {
            _trackModeListModel.toProject(_project);
        }
        setMode(Mode(key.function()));
        event.consume();
    }

    ListPage::keyPress(event);
}

void LayoutPage::setMode(Mode mode) {
    switch (mode) {
    case Mode::TrackMode:
        setListModel(_trackModeListModel);
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
