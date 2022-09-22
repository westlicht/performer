#include "TextInputPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"
#include "core/utils/StringUtils.h"

#include <cstring>

static const char characterSet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '-', ' '
};

enum class Function {
    Backspace   = 0,
    Delete      = 1,
    Clear       = 2,
    Cancel      = 3,
    OK          = 4,
};

static const char *functionNames[] = { "BS", "DEL", "CLEAR", "CANCEL", "OK" };


TextInputPage::TextInputPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void TextInputPage::show(const char *title, const char *text, size_t maxTextLength, ResultCallback callback) {
    _title = title;
    StringUtils::copy(_text, text, sizeof(_text));
    _maxTextLength = std::min(sizeof(_text) - 1, maxTextLength);
    _callback = callback;

    _selectedIndex = 0;
    _cursorIndex = std::strlen(_text);

    BasePage::show();
}

void TextInputPage::enter() {
}

void TextInputPage::exit() {
}

void TextInputPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);
    canvas.setFont(Font::Small);

    const int titleX = 28;
    const int titleY = 12;

    const int charsX = 28;
    const int charsY = 20;

    canvas.drawText(titleX, titleY, _title);
    int titleWidth = canvas.textWidth(_title) + 8;

    canvas.drawText(titleX + titleWidth, titleY, _text);

    int offset = 0;
    int width = 0;
    for (int i = 0; i <= _cursorIndex; ++i) {
        const char str[2] = { _text[i] == '\0' ? ' ' : _text[i], '\0' };
        width = canvas.textWidth(str);
        if (i < _cursorIndex) {
            offset += canvas.textWidth(str);
        }
    }

    if (os::ticks() % os::time::ms(300) < os::time::ms(150)) {
        canvas.setColor(Color::Medium);
        canvas.fillRect(titleX + titleWidth + offset, titleY - 8, width - 1, 12);
        const char str[2] = { _text[_cursorIndex], '\0' };
        canvas.setBlendMode(BlendMode::Sub);
        canvas.drawText(titleX + titleWidth + offset, titleY, str);
        canvas.setBlendMode(BlendMode::Set);
    }

    canvas.setFont(Font::Tiny);
    canvas.setColor(Color::Bright);

    int ix = 0;
    int iy = 0;
    for (int i = 0; i < int(sizeof(characterSet)); ++i) {
        canvas.drawTextCentered(charsX + ix * 10, charsY + iy * 10, 10, 10, FixedStringBuilder<2>("%c", characterSet[i]));
        if (_selectedIndex == i) {
            canvas.setColor(pageKeyState()[Key::Encoder] ? Color::Bright : Color::Medium);
            canvas.drawRect(charsX + ix * 10, charsY + iy * 10 + 1, 9, 9);
            canvas.setColor(Color::Bright);
        }
        ++ix;
        if (ix % 20 == 0) {
            ix = 0;
            ++iy;
        }
    }
}

void TextInputPage::updateLeds(Leds &leds) {
}

void TextInputPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Backspace:
            backspace();
            break;
        case Function::Delete:
            del();
            break;
        case Function::Clear:
            clear();
            break;
        case Function::Cancel:
            closeWithResult(false);
            break;
        case Function::OK:
            closeWithResult(true);
            break;
        }
    }

    if (key.isLeft()) {
        moveLeft();
    }

    if (key.isRight()) {
        moveRight();
    }

    if (key.is(Key::Encoder)) {
        insert(characterSet[_selectedIndex]);
    }

    event.consume();
}

void TextInputPage::encoder(EncoderEvent &event) {
    _selectedIndex = (_selectedIndex + event.value() + sizeof(characterSet)) % sizeof(characterSet);
}

void TextInputPage::closeWithResult(bool result) {
    Page::close();
    if (_callback) {
        _callback(result, _text);
    }
}

void TextInputPage::clear() {
    _cursorIndex = 0;
    _text[0] = '\0';
}

void TextInputPage::insert(char c) {
    if (_cursorIndex < _maxTextLength && int(std::strlen(_text)) < _maxTextLength) {
        for (int i = _maxTextLength; i > _cursorIndex; --i) {
            _text[i] = _text[i - 1];
        }
        _text[_cursorIndex] = c;
        ++_cursorIndex;
    }
}

void TextInputPage::backspace() {
    if (_cursorIndex > 0) {
        --_cursorIndex;
        for (int i = _cursorIndex; i < _maxTextLength; ++i) {
            _text[i] = _text[i + 1];
        }
    }
}

void TextInputPage::del() {
    if (_text[_cursorIndex] != '\0') {
        for (int i = _cursorIndex; i < _maxTextLength; ++i) {
            _text[i] = _text[i + 1];
        }
    }
}

void TextInputPage::moveLeft() {
    _cursorIndex = std::max(0, _cursorIndex - 1);
}

void TextInputPage::moveRight() {
    if (_text[_cursorIndex] != '\0') {
        ++_cursorIndex;
    }
}
