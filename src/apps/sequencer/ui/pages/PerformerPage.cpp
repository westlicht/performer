#include "PerformerPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"
#include "ui/layouts/GridLayout.h"
#include "ui/layouts/CenterLayout.h"

#include "core/utils/StringBuilder.h"

PerformerPage::PerformerPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void PerformerPage::enter() {
}

void PerformerPage::exit() {
}

void PerformerPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PERFORMER");

    const char *functionNames[] = { "M.ALL", "U.ALL", "CANCEL", nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    GridLayout<
        8,                  // CellCount
        16,                 // CellWidth
        16,                 // CellHeight
        4,                  // CellSpacing
        8                   // Padding
    > gridLayout;

    CenterLayout<
        Width,              // ContainerWidth
        Height,             // ContainerHeight
        gridLayout.width(), // Width
        gridLayout.height() // Height
    > centerLayout;

    constexpr int Border = 4;

    WindowPainter::drawFrame(canvas, centerLayout.originX(), centerLayout.originY(), centerLayout.width(), centerLayout.height());

    canvas.setBlendMode(BlendMode::Set);

    for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
        const auto &track = _engine.track(trackIndex);

        int x = centerLayout.originX() + gridLayout.cellX(trackIndex);
        int y = centerLayout.originY() + gridLayout.cellY(trackIndex);
        int w = gridLayout.cellWidth();
        int h = gridLayout.cellHeight();

        canvas.setColor(track.gate() ? 0xf : 0x7);
        canvas.drawRect(x, y, w, h);
        canvas.setColor(track.muted() ? 0xf : 0);
        canvas.fillRect(x + Border, y + Border, w - 2 * Border, h - 2 * Border);
    }
}

void PerformerPage::updateLeds(Leds &leds) {
}

void PerformerPage::keyDown(KeyEvent &event) {
}

void PerformerPage::keyUp(KeyEvent &event) {
}

void PerformerPage::encoder(EncoderEvent &event) {
}
