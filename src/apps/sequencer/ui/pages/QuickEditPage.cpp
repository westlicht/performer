#include "QuickEditPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

QuickEditPage::QuickEditPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void QuickEditPage::show(ListModel &listModel, int row) {
    _listModel = &listModel;
    _row = row;
    BasePage::show();
}

void QuickEditPage::enter() {
}

void QuickEditPage::exit() {
}

void QuickEditPage::draw(Canvas &canvas) {
    WindowPainter::drawFrame(canvas, 16, 16, 256 - 32, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Small);
    canvas.setColor(0xf);

    FixedStringBuilder<16> str;
    _listModel->cell(_row, 0, str);

    canvas.drawText(32, 34, str);

    str.reset();
    _listModel->cell(_row, 1, str);
    canvas.drawText(128 + 16, 34, str);
}

void QuickEditPage::updateLeds(Leds &leds) {
}

void QuickEditPage::keyDown(KeyEvent &event) {
    event.consume();
}

void QuickEditPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    // TODO
    if (key.isPage() || key.isStep()) {
        close();
        return;
    }

    event.consume();
}

void QuickEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.is(Key::Left)) {
        _listModel->edit(_row, 1, -1, key.shiftModifier());
    } else if (key.is(Key::Right)) {
        _listModel->edit(_row, 1, 1, key.shiftModifier());
    }
}

void QuickEditPage::encoder(EncoderEvent &event) {
    _listModel->edit(_row, 1, event.value(), event.pressed() | _keyState[Key::Shift]);
    event.consume();
}
