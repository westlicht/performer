#include "ListPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/math/Math.h"

static void drawScrollbar(Canvas &canvas, int x, int y, int w, int h, int totalRows, int visibleRows, int displayRow) {
    if (visibleRows >= totalRows) {
        return;
    }

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0x7);
    canvas.drawRect(x, y, w, h);

    int bh = (visibleRows * h) / totalRows;
    int by = (displayRow * h) / totalRows;
    canvas.setColor(0xf);
    canvas.fillRect(x, y + by, w, bh);
}

ListPage::ListPage(PageManager &manager, PageContext &context, ListModel &listModel) :
    BasePage(manager, context),
    _listModel(listModel)
{}

void ListPage::show() {
    scrollTo(_selectedRow);
    BasePage::show();
}

void ListPage::enter() {
}

void ListPage::exit() {
}

void ListPage::draw(Canvas &canvas) {
    canvas.setFont(Font::Small);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);

    FixedStringBuilder<32> str;

    for (int i = 0; i < 3; ++i) {
        int row = _displayRow + i;
        if (row < _listModel.rows()) {
            str.reset();
            _listModel.cell(row, 0, str);
            canvas.setColor(!_edit && row == _selectedRow ? 0xf : 0x7);
            canvas.drawText(8, 8 + (i + 1) * 14, str);

            str.reset();
            _listModel.cell(row, 1, str);
            canvas.setColor(_edit && row == _selectedRow ? 0xf : 0x7);
            canvas.drawText(128, 8 + (i + 1) * 14, str);
        }
    }

    drawScrollbar(canvas, Width - 8, 10, 4, 3 * 14, _listModel.rows(), 3, _displayRow);
}

void ListPage::updateLeds(Leds &leds) {
}

void ListPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    switch (key.code()) {
    case Key::Left:
        _edit = false;
        event.consume();
        break;
    case Key::Right:
        _edit = true;
        event.consume();
        break;
    case Key::Encoder:
        _edit = !_edit;
        event.consume();
        break;
    }
}

void ListPage::encoder(EncoderEvent &event) {
    if (_edit) {
        _listModel.edit(_selectedRow, 1, event.value());
    } else {
        setSelectedRow(selectedRow() + event.value());
    }
    event.consume();
}

void ListPage::setSelectedRow(int selectedRow) {
    if (selectedRow != _selectedRow) {
        _selectedRow = clamp(selectedRow, 0, _listModel.rows() - 1);;
        scrollTo(_selectedRow);
    }
}

void ListPage::scrollTo(int row) {
    if (row < _displayRow) {
        _displayRow = row;
    } else if (row >= _displayRow + 3) {
        _displayRow = row - 2;
    }
}
