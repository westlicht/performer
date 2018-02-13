#include "BpmPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

BpmPage::BpmPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void BpmPage::enter() {
    _engine.tapTempoReset();
}

void BpmPage::exit() {
}

void BpmPage::draw(Canvas &canvas) {
    WindowPainter::drawFrame(canvas, 16, 16, 256 - 32, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Small);

    FixedStringBuilder<16> string("BPM: %.1f", _project.bpm());
    canvas.drawText(50, 36, string);
}

void BpmPage::updateLeds(Leds &leds) {
}

void BpmPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.is(Key::Start)) {
        // tap tempo
        _engine.tapTempoTap();
    } else if (key.is(Key::Left)) {
        // notch tempo down
    } else if (key.is(Key::Right)) {
        // notch tempo up
    }

    event.consume();
}

void BpmPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.is(Key::BPM)) {
        close();
    } else if (key.is(Key::Left) || key.is(Key::Right)) {
        // reset notch
    }

    event.consume();
}

void BpmPage::encoder(EncoderEvent &event) {
    _project.setBpm(_project.bpm() + event.value() * (event.pressed() ? 0.1f : 1.f) * (_keyState[Key::Shift] ? 10.f : 1.f));

    event.consume();
}
