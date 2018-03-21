#include "CurveSequenceSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

CurveSequenceSetupPage::CurveSequenceSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void CurveSequenceSetupPage::enter() {
    _listModel.setSequence(&_project.selectedCurveSequence());
}

void CurveSequenceSetupPage::exit() {
    _listModel.setSequence(nullptr);
}

void CurveSequenceSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void CurveSequenceSetupPage::updateLeds(Leds &leds) {
}
