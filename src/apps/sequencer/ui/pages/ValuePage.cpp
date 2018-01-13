#include "ValuePage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

ValuePage::ValuePage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void ValuePage::show(const Handler &handler) {
    _handler = handler;
    _manager.push(this);
}

void ValuePage::enter() {
}

void ValuePage::exit() {
}

void ValuePage::draw(Canvas &canvas) {
    WindowPainter::drawFrame(canvas, 10, 16, 256 - 20, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Small);
    FixedStringBuilder<32> string;
    _handler.value(string);
    canvas.drawText(50, 36, string);
}

void ValuePage::updateLeds(Leds &leds) {
}

void ValuePage::keyDown(KeyEvent &event) {
    if (event.key().isGlobal()) {
        return;
    }

    event.consume();
}

void ValuePage::keyUp(KeyEvent &event) {
    if (event.key().isGlobal()) {
        return;
    }

    if (event.key().is(_handler.exitKey)) {
        close();
    }

    event.consume();
}

void ValuePage::encoder(EncoderEvent &event) {
    _handler.encoder(event.value(), event.pressed(), _context.keyState[Key::Shift]);
    event.consume();
}
