#include "MenuPage.h"

#include "core/utils/StringBuilder.h"

#include "ui/painters/WindowPainter.h"

MenuPage::MenuPage(PageManager &manager, PageContext &context, Options &options) :
    BasePage(manager, context),
    _options(options)
{
}

void MenuPage::enter() {
}

void MenuPage::exit() {
}

void MenuPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);

    canvas.setColor(0xf);
    canvas.hline(0, 16, Width);
    canvas.hline(0, Height - 16, Width);

    WindowPainter::drawFunctionKey(canvas, 0, "PREV", _keyState[Key::F0]);
    WindowPainter::drawFunctionKey(canvas, 1, "NEXT", _keyState[Key::F1]);
    WindowPainter::drawFunctionKey(canvas, 4, "EXIT", _keyState[Key::F4]);

    canvas.setColor(0xf);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Tiny);
    canvas.drawText(4, 10, _options.title);

    FixedStringBuilder<16> selectedItem("%d/%d", _selectedItem + 1, _options.itemCount);
    canvas.drawText(Width - 4 - canvas.textWidth(selectedItem), 10, selectedItem);

    auto itemX = [] (int index, int count) -> int {
        return 4 + ((Width - 8) * index) / count;
    };

    int x0 = itemX(_selectedItem, _options.itemCount) + 1;
    int x1 = itemX(_selectedItem + 1, _options.itemCount) - 1;
    canvas.hline(x0 + 1, Height - 15, x1 - x0 - 2);
    canvas.hline(x0 + 1, Height - 17, x1 - x0 - 2);
}

void MenuPage::updateLeds(Leds &leds) {
}

void MenuPage::keyDown(KeyEvent &event) {
    event.consume();
}

void MenuPage::keyUp(KeyEvent &event) {
    switch (event.key().code()) {
    case Key::F0:
        _selectedItem -= _selectedItem > 0 ? 1 : 0;
        break;
    case Key::F1:
        _selectedItem += _selectedItem < _options.itemCount - 1 ? 1 : 0;
        break;
    case Key::F4:
        close();
        break;
    default:
        break;
    }

    event.consume();
}

void MenuPage::encoder(EncoderEvent &event) {
    if (event.pressed()) {
        _selectedItem += event.value();
    }

    event.consume();
}

void MenuPage::drawItemName(Canvas &canvas, const char *name) {
    canvas.setColor(0xf);
    canvas.setFont(Font::Small);
    canvas.drawText(4, 36, name);
}

void MenuPage::drawItemValue(Canvas &canvas, const char *value) {
    canvas.setColor(0xf);
    canvas.setFont(Font::Small);
    canvas.drawText(128, 36, value);
}

void MenuPage::drawItemValue(Canvas &canvas, int value, const char *unit) {
    if (unit) {
        drawItemValue(canvas, FixedStringBuilder<16>("%d%s", value, unit));
    } else {
        drawItemValue(canvas, FixedStringBuilder<16>("%d", value));
    }
}

void MenuPage::drawItemValue(Canvas &canvas, bool value) {
    drawItemValue(canvas, value ? "On" : "Off");
}
