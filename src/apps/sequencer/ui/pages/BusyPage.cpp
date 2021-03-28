#include "BusyPage.h"

#include "ui/painters/WindowPainter.h"

static void drawProgressBar(Canvas &canvas, int x, int y, int w, int h, int stripeLength, int stripeOffset) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);

    int sx = (stripeOffset % stripeLength) - stripeLength;
    while (sx < w + stripeLength) {
        int sx0 = std::max(0, sx);
        int sx1 = std::min(w, sx + stripeLength / 2);
        canvas.fillRect(x + sx0, y, sx1 - sx0, h);
        sx += stripeLength;
    }
}

BusyPage::BusyPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void BusyPage::show(const char *text) {
    _text = text;
    BasePage::show();
}

void BusyPage::enter() {
}

void BusyPage::exit() {
}

void BusyPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);

    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);

    canvas.drawTextCentered(0, 32 - 16, Width, 8, _text);

    drawProgressBar(canvas, 16, 32 - 4, Width - 32, 8, 16, (os::ticks() / os::time::ms(50)) % 16);
}

void BusyPage::updateLeds(Leds &leds) {
}
