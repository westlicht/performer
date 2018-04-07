#include "ProjectSelectPage.h"

#include "ui/painters/WindowPainter.h"

enum class Function {
    Cancel  = 3,
    OK      = 4,
};

static const char *functionNames[] = { nullptr, nullptr, nullptr, "CANCEL", "OK" };


ProjectSelectPage::ProjectSelectPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void ProjectSelectPage::show(const char *title, int selectedSlot, bool allowEmpty, ResultCallback callback) {
    _title = title;
    _allowEmpty = allowEmpty;
    _callback = callback;
    setSelectedRow(clamp(selectedSlot, 0, _listModel.rows() - 1));
    ListPage::show();
}

void ProjectSelectPage::enter() {
}

void ProjectSelectPage::exit() {
}

void ProjectSelectPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, _title);
    WindowPainter::drawFooter(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void ProjectSelectPage::updateLeds(Leds &leds) {
}

void ProjectSelectPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Cancel:
            closeWithResult(false);
            break;
        case Function::OK:
            closeWithResult(true);
            break;
        }
    }

    if (key.is(Key::Encoder)) {
        closeWithResult(true);
        return;
    }

    ListPage::keyPress(event);
}

void ProjectSelectPage::closeWithResult(bool result) {
    // cancel if empty slot is selected but not allowed to be
    if (result && !_allowEmpty) {
        ProjectManager::SlotInfo info;
        ProjectManager::slotInfo(selectedRow(), info);
        if (!info.used) {
            return;
        }
    }

    Page::close();
    if (_callback) {
        _callback(result, selectedRow());
    }
}
