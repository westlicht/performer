#include "QuickEditPage.h"

#include "ui/LedPainter.h"
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
    canvas.setColor(Color::Bright);

    FixedStringBuilder<16> str;
    _listModel->cell(_row, 0, str);

    canvas.drawText(32, 34, str);

    str.reset();
    _listModel->cell(_row, 1, str);
    canvas.drawText(128 + 16, 34, str);
}

void QuickEditPage::updateLeds(Leds &leds) {
    leds.clear();
    LedPainter::drawSelectedQuickEditValue(leds, _listModel->indexed(_row), _listModel->indexedCount(_row));
}

void QuickEditPage::keyDown(KeyEvent &event) {
    event.consume();
}

void QuickEditPage::keyUp(KeyEvent &event) {
    event.consume();

    if (globalKeyState()[Key::Page]) {
        return;
    }
    for (int i = 8; i < 16; ++i) {
        if (globalKeyState()[MatrixMap::fromStep(i)]) {
            return;
        }
    }

    close();
}

void QuickEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isLeft()) {
        _listModel->edit(_row, 1, -1, key.shiftModifier());
    } else if (key.isRight()) {
        _listModel->edit(_row, 1, 1, key.shiftModifier());
    } else if (key.isStep()) {
        _listModel->setIndexed(_row, key.step());
    }
}

void QuickEditPage::encoder(EncoderEvent &event) {
    _listModel->edit(_row, 1, event.value(), event.pressed() | globalKeyState()[Key::Shift]);
    event.consume();
}
