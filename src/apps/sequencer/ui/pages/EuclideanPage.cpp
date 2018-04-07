#include "EuclideanPage.h"

#include "ui/painters/WindowPainter.h"

enum class Function {
    Steps   = 0,
    Beats   = 1,
    Offset  = 2,
};

static const char *functionNames[] = { "STEPS", "BEATS", "OFFSET", nullptr, nullptr };


EuclideanPage::EuclideanPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void EuclideanPage::enter() {
    updatePattern();
}

void EuclideanPage::exit() {
}

void EuclideanPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawFooter(canvas, functionNames, _keyState);

    canvas.setFont(Font::Small);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);

    auto drawValue = [&] (int index, const char *str) {
        int w = Width / 5;
        int x = (Width * index) / 5;
        int y = Height - 16;
        canvas.drawText(x + (w - canvas.textWidth(str)) / 2, y, str);
    };

    drawValue(0, FixedStringBuilder<8>("%d", _steps));
    drawValue(1, FixedStringBuilder<8>("%d", _beats));
    drawValue(2, FixedStringBuilder<8>("%d", _offset));

    int stepWidth = Width / _steps;
    int stepHeight = 8;
    int x = (Width - _steps * stepWidth) / 2;
    int y = 16;

    for (int i = 0; i < _steps; ++i) {
        canvas.setColor(0x7);
        canvas.drawRect(x + 1, y + 1, stepWidth - 2, stepHeight - 2);
        if (_pattern[i]) {
            canvas.setColor(0xf);
            canvas.fillRect(x + 1, y + 1, stepWidth - 2, stepHeight - 2);
        }
        x += stepWidth;
    }
}

void EuclideanPage::updateLeds(Leds &leds) {
}

void EuclideanPage::keyPress(KeyPressEvent &event) {
}

void EuclideanPage::encoder(EncoderEvent &event) {
    if (_keyState[Key::F0]) {
        _steps = clamp(_steps + event.value(), 1, CONFIG_STEP_COUNT);
        _beats = clamp(_beats, 0, _steps);
    } else if (_keyState[Key::F1]) {
        _beats = clamp(_beats + event.value(), 0, _steps);
    } else if (_keyState[Key::F2]) {
        _offset = clamp(_offset + event.value(), 0, CONFIG_STEP_COUNT - 1);
    }
    updatePattern();
}

void EuclideanPage::updatePattern() {
    _pattern = Rhythm::euclidean(_beats, _steps).shifted(_offset);
}
