#include "ProjectPage.h"

#include "ui/LedPainter.h"
#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "model/ProjectManager.h"

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

    const char *functionNames[] = { "LOAD", "SAVE", "SAVEAS", "FORMAT", nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void ProjectPage::updateLeds(Leds &leds) {
}

void ProjectPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (key.function()) {
        case 0: loadProject(); break;
        case 1: saveProject(); break;
        case 2: saveAsProject(); break;
        case 3: formatSDCard(); break;
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

    ListPage::keyPress(event);
}

void ProjectPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}

void ProjectPage::loadProject() {
    _manager.pages().projectSelect.show("LOAD PROJECT", _project.slotAssigned() ? _project.slot() : 0, false, [this] (bool result, int index) {
        if (result) {
            auto result = ProjectManager::loadProject(_project, index);
            if (result == fs::OK) {
                showMessage(FixedStringBuilder<32>("Loaded project"));
            } else {
                showMessage(FixedStringBuilder<32>("Loading project failed (%s)", fs::errorToString(result)));
            }
        }
    });
}

void ProjectPage::saveProject() {
    if (!_project.slotAssigned()) {
        saveAsProject();
        return;
    }

    auto result = ProjectManager::saveProject(_project, _project.slot());
    if (result == fs::OK) {
        showMessage(FixedStringBuilder<32>("Saved project"));
    } else {
        showMessage(FixedStringBuilder<32>("Saving project failed (%s)", fs::errorToString(result)));
    }
}

void ProjectPage::saveAsProject() {
    _manager.pages().projectSelect.show("SAVE PROJECT", 0, true, [this] (bool result, int index) {
        if (result) {
            auto result = ProjectManager::saveProject(_project, index);
            if (result == fs::OK) {
                showMessage(FixedStringBuilder<32>("Saved project"));
            } else {
                showMessage(FixedStringBuilder<32>("Saving project failed (%s)", fs::errorToString(result)));
            }
        }
    });
}

void ProjectPage::formatSDCard() {
    _manager.pages().confirmation.show("DO YOU REALLY WANT TO FORMAT THE SDCARD?", [this] (bool result) {
        if (result) {
            _manager.pages().busy.show("FORMATTING ...");
            auto result = ProjectManager::format();
            if (result != fs::OK) {
                showMessage(FixedStringBuilder<32>("FORMAT FAILED (%s)", fs::errorToString(result)));
            }
            _manager.pages().busy.close();
        }
    });
}
