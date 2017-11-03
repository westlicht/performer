#include "MutePage.h"

#include "ui/LedPainter.h"

#include "ui/painters/WindowPainter.h"

#include "ui/layouts/CenterLayout.h"
#include "ui/layouts/GridLayout.h"

MutePage::MutePage(PageManager &pageManager, PageContext &context) :
    Page(pageManager),
    _model(context.model),
    _engine(context.engine)
{}

void MutePage::enter() {
}

void MutePage::exit() {
}

void MutePage::draw(Canvas &canvas) {

    GridLayout<
        8,                  // CellCount
        16,                 // CellWidth
        16,                 // CellHeight
        4,                  // CellSpacing
        8                   // Padding
    > gridLayout;

    CenterLayout<
        PageWidth,          // ContainerWidth
        PageHeight,         // ContainerHeight
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

void MutePage::updateLeds(Leds &leds) {
    LedPainter::drawTracksGateAndMuted(leds, _engine);
}

void MutePage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isGlobal()) {
        return;
    }

    if (key.isTrack()) {
        _engine.track(key.track()).toggleMuted();
    }

    event.consume();
}

void MutePage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isGlobal()) {
        return;
    }

    if (key.is(Key::Mute)) {
        close();
    }

    event.consume();
}

void MutePage::encoder(EncoderEvent &event) {
    event.consume();
}
