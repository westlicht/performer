#include "ProjectPage.h"

#include "ui/LedPainter.h"
#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "core/fs/FileSystem.h"
#include "core/utils/StringBuilder.h"

ProjectPage::ProjectPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
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
    }
}

void ProjectPage::keyUp(KeyEvent &event) {
}

void ProjectPage::encoder(EncoderEvent &event) {
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
