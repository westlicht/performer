#include "TextInputPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

#include <cstring>

static const char characterSet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '-', ' '
};

TextInputPage::TextInputPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void TextInputPage::enter() {
    _selectedIndex = 0;
    _cursorIndex = 0;
    _text[0] = '\0';
}

void TextInputPage::exit() {
}

void TextInputPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);

    const char *functionNames[] = { "BS", "DEL", "CLEAR", "CANCEL", "OK" };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.setFont(Font::Tiny);

    canvas.drawText(28, 8, _title);
    int titleWidth = canvas.textWidth(_title) + 8;

    canvas.drawText(28 + titleWidth, 8, _text);

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
        canvas.fillRect(28 + titleWidth + offset - 1, 8 - 5, width + 1, 7);
        const char str[2] = { _text[_cursorIndex], '\0' };
        canvas.setBlendMode(BlendMode::Sub);
        canvas.drawText(28 + titleWidth + offset, 8, str);
        canvas.setBlendMode(BlendMode::Set);
    }




    int ix = 0;
    int iy = 0;
    for (int i = 0; i < int(sizeof(characterSet)); ++i) {
        canvas.drawTextCentered(28 + ix * 10, 16 + iy * 10, 10, 10, FixedStringBuilder<2>("%c", characterSet[i]));
        if (_selectedIndex == i) {
            canvas.setColor(_keyState[Key::Encoder] ? 0xf : 0x7);
            canvas.drawRect(28 + ix * 10, 16 + iy * 10 + 1, 9, 9);
            canvas.setColor(0xf);
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

void TextInputPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch(key.function()) {
        case 0: backspace(); break;
        case 1: del(); break;
        case 2: clear(); break;
        case 3: close(false); break;
        case 4: close(true); break;
        }
    }

    if (key.is(Key::Left)) {
        moveLeft();
    }

    if (key.is(Key::Right)) {
        moveRight();
    }

    if (key.is(Key::Encoder)) {
        insert(characterSet[_selectedIndex]);
    }

    event.consume();
}

void TextInputPage::keyUp(KeyEvent &event) {
}

void TextInputPage::encoder(EncoderEvent &event) {
    _selectedIndex = (_selectedIndex + event.value() + sizeof(characterSet)) % sizeof(characterSet);
}

void TextInputPage::close(bool result) {
    Page::close();
    // TODO callback
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
