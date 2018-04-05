#include "CurveSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

CurveSequencePage::CurveSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
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
