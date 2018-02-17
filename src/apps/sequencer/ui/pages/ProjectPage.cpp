#include "ProjectPage.h"

#include "ui/LedPainter.h"
#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "core/fs/FileSystem.h"
#include "core/utils/StringBuilder.h"

ProjectPage::ProjectPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _listModel(context.model.project())
{}

void ProjectPage::enter() {
}

void ProjectPage::exit() {
}

void ProjectPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PROJECT");

    const char *functionNames[] = { "LOAD", "SAVE", "FORMAT", nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void ProjectPage::updateLeds(Leds &leds) {
}

void ProjectPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (key.function()) {
        case 0: loadProject(); break;
        case 1: saveProject(); break;
        case 2: formatSDCard(); break;
        }
        return;
    }

    if (key.is(Key::Encoder) && _selectedRow == 0) {
        _manager.pages().textInput.show("NAME:", _project.name(), Project::NameLength, [this] (bool result, const char *text) {
            if (result) {
                _project.setName(text);
            }
        });

        return;
    }

    ListPage::keyDown(event);
}

void ProjectPage::keyUp(KeyEvent &event) {
    ListPage::keyUp(event);
}

void ProjectPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}

void ProjectPage::loadProject() {
    auto result = fs::volume().mount();
    if (result != fs::OK) {
        showMessage(FixedStringBuilder<32>("MOUNT FAILED (%s)", fs::errorToString(result)));
        return;
    }

    result = _model.read("test.pro");
    if (result != fs::OK) {
        showMessage(FixedStringBuilder<32>("LOAD FAILED (%s)", fs::errorToString(result)));
    }
}

void ProjectPage::saveProject() {
    auto result = fs::volume().mount();
    if (result != fs::OK) {
        showMessage(FixedStringBuilder<32>("MOUNT FAILED (%s)", fs::errorToString(result)));
        return;
    }

    result = _model.write("test.pro");
    if (result != fs::OK) {
        showMessage(FixedStringBuilder<32>("SAVE FAILED (%s)", fs::errorToString(result)));
    }
}

void ProjectPage::formatSDCard() {
    _manager.pages().confirmation.show("DO YOU REALLY WANT TO FORMAT THE SDCARD?", [this] (bool result) {
        if (result) {
            _manager.pages().busy.show("FORMATTING ...");
            auto result = fs::volume().format();
            if (result != fs::OK) {
                showMessage(FixedStringBuilder<32>("FORMAT FAILED (%s)", fs::errorToString(result)));
            }
            _manager.pages().busy.close();
        }
    });
}
