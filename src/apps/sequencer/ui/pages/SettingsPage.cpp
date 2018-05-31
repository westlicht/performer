#include "SettingsPage.h"

#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

#ifdef PLATFORM_STM32
#include "drivers/System.h"
#endif

enum Function {
    Calibration = 0,
    Update      = 4,
};

static const char *functionNames[] = { "CAL", nullptr, nullptr, nullptr, "UPDATE" };

enum class ContextAction {
    Init,
    Save,
    Backup,
    Restore,
    Format,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "SAVE" },
    { "BACKUP" },
    { "RESTORE" },
    { "FORMAT" },
};

SettingsPage::SettingsPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _cvOutputListModel),
    _settings(context.model.settings())
{
    setOutputIndex(_project.selectedTrackIndex());
}

void SettingsPage::enter() {
    _engine.lock();
    _engine.setGateOutput(0xff);
    _engine.setGateOutputOverride(true);
    _engine.setCvOutputOverride(true);

    _encoderDownTicks = 0;

    updateOutputs();
}

void SettingsPage::exit() {
    _engine.setGateOutputOverride(false);
    _engine.setCvOutputOverride(false);
    _engine.unlock();
}

void SettingsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SETTINGS");
    WindowPainter::drawFooter(canvas, functionNames, _keyState);

    switch (_mode) {
    case Mode::Calibration: {
        FixedStringBuilder<8> str("CAL CV%d", _outputIndex + 1);
        WindowPainter::drawActiveFunction(canvas, str);
        ListPage::draw(canvas);
        break;
    }
    case Mode::Update: {
        WindowPainter::drawActiveFunction(canvas, "UPDATE");
        canvas.setColor(0xf);
        canvas.drawText(4, 24, "CURRENT VERSION:");
        FixedStringBuilder<16> str("%d.%d.%d", CONFIG_VERSION_MAJOR, CONFIG_VERSION_MINOR, CONFIG_VERSION_REVISION);
        canvas.drawText(100, 24, str);
        canvas.drawText(4, 40, "PRESS AND HOLD ENCODER TO RESET TO BOOTLOADER");

#ifdef PLATFORM_STM32
        if (_encoderDownTicks != 0 && os::ticks() - _encoderDownTicks > os::time::ms(2000)) {
            System::reset();
        }
#endif

        break;
    }
    }
}

void SettingsPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    switch (_mode) {
    case Mode::Update:
        if (key.isEncoder()) {
            _encoderDownTicks = os::ticks();
        }
        break;
    default:
        break;
    }
}

void SettingsPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    switch (_mode) {
    case Mode::Update:
        if (key.isEncoder()) {
            _encoderDownTicks = 0;
        }
        break;
    default:
        break;
    }
}

void SettingsPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Calibration:
            setMode(Mode::Calibration);
            break;
        case Function::Update:
            setMode(Mode::Update);
            break;
        }
    }

    switch (_mode) {
    case Mode::Calibration:
        if (key.isTrack()) {
            setOutputIndex(key.track());
        }
        ListPage::keyPress(event);
        updateOutputs();
        break;
    case Mode::Update:
        break;
    }
}

void SettingsPage::encoder(EncoderEvent &event) {
    switch (_mode) {
    case Mode::Calibration:
        ListPage::encoder(event);
        updateOutputs();
        break;
    case Mode::Update:
        break;
    }
}

void SettingsPage::setMode(Mode mode) {
    _mode = mode;
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
    case ContextAction::Save:
        saveSettings();
        break;
    case ContextAction::Backup:
        backupSettings();
        break;
    case ContextAction::Restore:
        restoreSettings();
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
    case ContextAction::Backup:
    case ContextAction::Restore:
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

void SettingsPage::saveSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            saveSettingsToFlash();
        }
    });
}

void SettingsPage::backupSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            backupSettingsToFile();
        }
    });
}

void SettingsPage::restoreSettings() {
    if (fs::exists(Settings::Filename)) {
        _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
            if (result) {
                restoreSettingsFromFile();
            }
        });
    }
}

void SettingsPage::saveSettingsToFlash() {
    _engine.lock();
    _manager.pages().busy.show("SAVING SETTINGS ...");

    FileManager::task([this] () {
        _model.settings().writeToFlash();
        return fs::OK;
    }, [this] (fs::Error result) {
        showMessage("SETTINGS SAVED");
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.unlock();
    });
}

void SettingsPage::backupSettingsToFile() {
    _engine.lock();
    _manager.pages().busy.show("BACKING UP SETTINGS ...");

    FileManager::task([this] () {
        return _model.settings().write(Settings::Filename);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage("SETTINGS BACKED UP");
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.unlock();
    });
}

void SettingsPage::restoreSettingsFromFile() {
    _engine.lock();
    _manager.pages().busy.show("RESTORING SETTINGS ...");

    FileManager::task([this] () {
        return _model.settings().read(Settings::Filename);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage("SETTINGS RESTORED");
        } else if (result == fs::INVALID_CHECKSUM) {
            showMessage("INVALID SETTINGS FILE");
            _model.settings().readFromFlash();
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
            _model.settings().readFromFlash();
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.unlock();
    });
}

void SettingsPage::formatSdCard() {
    _manager.pages().confirmation.show("DO YOU REALLY WANT TO FORMAT THE SDCARD?", [this] (bool result) {
        if (result) {
            _manager.pages().busy.show("FORMATTING SDCARD ...");

            FileManager::task([] () {
                return FileManager::format();
            }, [this] (fs::Error result) {
                if (result == fs::OK) {
                    showMessage("SDCARD FORMATTED");
                } else {
                    showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
                }
                // TODO lock ui mutex
                _manager.pages().busy.close();
            });
        }
    });
}
