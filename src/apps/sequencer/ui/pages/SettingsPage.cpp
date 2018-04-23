#include "SettingsPage.h"

#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Load,
    Save,
    Format,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "LOAD" },
    { "SAVE" },
    { "FORMAT" },
};

SettingsPage::SettingsPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _cvOutputListModel),
    _settings(context.model.settings())
{
    setOutputIndex(_project.selectedTrackIndex());
}

void SettingsPage::enter() {
    _engine.setGateOutput(0xff);
    _engine.setGateOutputOverride(true);
    _engine.setCvOutputOverride(true);

    updateOutputs();
}

void SettingsPage::exit() {
    _engine.setGateOutputOverride(false);
    _engine.setCvOutputOverride(false);
}

void SettingsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SETTINGS");
    FixedStringBuilder<8> str("CV%d", _outputIndex + 1);
    WindowPainter::drawActiveFunction(canvas, str);
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}

void SettingsPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isTrack()) {
        setOutputIndex(key.track());
    }

    ListPage::keyPress(event);

    updateOutputs();
}

void SettingsPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);

    updateOutputs();
}

void SettingsPage::setOutputIndex(int index) {
    _outputIndex = index;
    _cvOutputListModel.setCvOutput(_settings.calibration().cvOutput(index));
}

void SettingsPage::updateOutputs() {
    float volts = Calibration::CvOutput::itemToVolts(selectedRow());
    for (int i = 0; i < CONFIG_CV_OUTPUT_CHANNELS; ++i) {
        _engine.setCvOutput(i, volts);
    }
}

void SettingsPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void SettingsPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initSettings();
        break;
    case ContextAction::Load:
        loadSettings();
        break;
    case ContextAction::Save:
        saveSettings();
        break;
    case ContextAction::Format:
        formatSdCard();
        break;
    case ContextAction::Last:
        break;
    }
}

bool SettingsPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Load:
    case ContextAction::Save:
        return FileManager::volumeMounted();
    case ContextAction::Format:
        return FileManager::volumeAvailable();
    default:
        return true;
    }
}

void SettingsPage::initSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            _settings.clear();
            showMessage("SETTINGS INITIALIZED");
        }
    });
}

void SettingsPage::loadSettings() {
    if (fs::exists(Settings::filename)) {
        _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
            if (result) {
                loadSettingsFromFile();
            }
        });
    }
}

void SettingsPage::saveSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            saveSettingsToFile();
        }
    });
}

void SettingsPage::formatSdCard() {
    _manager.pages().confirmation.show("DO YOU REALLY WANT TO FORMAT THE SDCARD?", [this] (bool result) {
        if (result) {
            _manager.pages().busy.show("FORMATTING ...");

            FileManager::task([] () {
                return FileManager::format();
            }, [this] (fs::Error result) {
                if (result == fs::OK) {
                    showMessage(FixedStringBuilder<32>("SDCARD FORMATTED"));
                } else {
                    showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
                }
                // TODO lock ui mutex
                _manager.pages().busy.close();
            });
        }
    });
}

void SettingsPage::loadSettingsFromFile() {
    _manager.pages().busy.show("LOADING SETTINGS ...");

    FileManager::task([this] () {
        return _model.settings().read(Settings::filename);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage(FixedStringBuilder<32>("LOADED SETTINGS"));
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
    });
}

void SettingsPage::saveSettingsToFile() {
    _manager.pages().busy.show("SAVING SETTINGS ...");

    FileManager::task([this] () {
        return _model.settings().write(Settings::filename);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage(FixedStringBuilder<32>("SAVED SETTINGS"));
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
    });
}
