#include "SystemPage.h"

#include "ui/pages/Pages.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

#ifdef PLATFORM_STM32
#include "drivers/System.h"
#endif

enum Function {
    Calibration = 0,
    Utilities   = 2,
    Update      = 3,
    Settings    = 4,
};

static const char *functionNames[] = { "CAL", nullptr, "UTILS", "UPDATE", "SETTINGS" };

enum CalibrationEditFunction {
    Auto        = 0,
};

static const char *calibrationEditFunctionNames[] = { "AUTO", nullptr, nullptr, nullptr, nullptr };

enum class ContextAction {
    Init,
    Save,
    Backup,
    Restore,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "SAVE" },
    { "BACKUP" },
    { "RESTORE" }
};

SystemPage::SystemPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _cvOutputListModel),
    _settings(context.model.settings()),
    _settingsListModel(_settings.userSettings())
{
    setOutputIndex(0);
}

void SystemPage::enter() {
    setOutputIndex(_project.selectedTrackIndex());

    _engine.suspend();
    _engine.setGateOutput(0xff);
    _engine.setGateOutputOverride(true);
    _engine.setCvOutputOverride(true);

    _encoderDownTicks = 0;

    updateOutputs();
}

void SystemPage::exit() {
    _engine.setGateOutputOverride(false);
    _engine.setCvOutputOverride(false);
    _engine.resume();
}

void SystemPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SYSTEM");

    switch (_mode) {
    case Mode::Calibration: {
        FixedStringBuilder<8> str("CAL CV%d", _outputIndex + 1);
        WindowPainter::drawActiveFunction(canvas, str);
        if (edit()) {
            WindowPainter::drawFooter(canvas, calibrationEditFunctionNames, pageKeyState());
        } else {
            WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), int(_mode));
        }
        ListPage::draw(canvas);
        break;
    }
    case Mode::Utilities: {
        WindowPainter::drawActiveFunction(canvas, "UTILITIES");
        WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), int(_mode));
        ListPage::draw(canvas);
        break;
    }
    case Mode::Update: {
        WindowPainter::drawActiveFunction(canvas, "UPDATE");
        WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), int(_mode));
        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::Bright);
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
    case Mode::Settings: {
        WindowPainter::drawActiveFunction(canvas, "SETTINGS");
        WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), int(_mode));
        ListPage::draw(canvas);
        break;
    }
    }
}

void SystemPage::updateLeds(Leds &leds) {
    int selectedTrack = _project.selectedTrackIndex();

    for (int track = 0; track < 8; ++track) {
        bool selected = track == selectedTrack;
        leds.set(MatrixMap::fromTrack(track), selected, selected);
    }

    for (int step = 0; step < 16; ++step) {
        leds.set(MatrixMap::fromStep(step), false, false);
    }
}

void SystemPage::keyDown(KeyEvent &event) {
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

void SystemPage::keyUp(KeyEvent &event) {
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

void SystemPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        if (_mode == Mode::Calibration || _mode == Mode::Settings) {
            contextShow();
            event.consume();
            return;
        }
    }

    if (key.isFunction()) {
        if (_mode == Mode::Calibration && edit()) {
            if (CalibrationEditFunction(key.function()) == CalibrationEditFunction::Auto) {
                _settings.calibration().cvOutput(_project.selectedTrackIndex()).setUserDefined(selectedRow(), false);
                setEdit(false);
            }
        } else {
            switch (Function(key.function())) {
            case Function::Calibration:
                setMode(Mode::Calibration);
                break;
            case Function::Utilities:
                setMode(Mode::Utilities);
                break;
            case Function::Update:
                setMode(Mode::Update);
                break;
            case Function::Settings:
                setMode(Mode::Settings);
                break;
            }
        }
    }

    switch (_mode) {
    case Mode::Calibration:
        if (key.isTrack()) {
            setOutputIndex(key.track());
        }
        if (!edit() && key.isEncoder()) {
            _settings.calibration().cvOutput(_project.selectedTrackIndex()).setUserDefined(selectedRow(), true);
        }
        ListPage::keyPress(event);
        updateOutputs();
        break;
    case Mode::Utilities:
        if (key.isEncoder()) {
            executeUtilityItem(UtilitiesListModel::Item(selectedRow()));
        } else {
            ListPage::keyPress(event);
        }
        break;
    case Mode::Update:
        break;
    case Mode::Settings:
        ListPage::keyPress(event);
        break;
    }
}

void SystemPage::encoder(EncoderEvent &event) {
    switch (_mode) {
    case Mode::Calibration:
        ListPage::encoder(event);
        updateOutputs();
        break;
    case Mode::Utilities:
        ListPage::encoder(event);
        break;
    case Mode::Update:
        break;
    case Mode::Settings:
        ListPage::encoder(event);
        break;
    }
}

void SystemPage::setMode(Mode mode) {
    _mode = mode;
    switch (_mode) {
    case Mode::Calibration:
        setListModel(_cvOutputListModel);
        break;
    case Mode::Utilities:
        setListModel(_utilitiesListModel);
        break;
    case Mode::Settings:
        setListModel(_settingsListModel);
        break;
    default:
        break;
    }
}

void SystemPage::setOutputIndex(int index) {
    _outputIndex = index;
    _cvOutputListModel.setCvOutput(_settings.calibration().cvOutput(index));
}

void SystemPage::updateOutputs() {
    float volts = Calibration::CvOutput::itemToVolts(selectedRow());
    for (int i = 0; i < CONFIG_CV_OUTPUT_CHANNELS; ++i) {
        _engine.setCvOutput(i, volts);
    }
}

void SystemPage::executeUtilityItem(UtilitiesListModel::Item item) {
    switch (item) {
    case UtilitiesListModel::FormatSdCard:
        formatSdCard();
        break;
    case UtilitiesListModel::Last:
        break;
    }
}

void SystemPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void SystemPage::contextAction(int index) {
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
    case ContextAction::Last:
        break;
    }
}

bool SystemPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Backup:
    case ContextAction::Restore:
        return FileManager::volumeMounted();
    default:
        return true;
    }
}

void SystemPage::initSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            _settings.clear();
            showMessage("SETTINGS INITIALIZED");
        }
    });
}

void SystemPage::saveSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            saveSettingsToFlash();
        }
    });
}

void SystemPage::backupSettings() {
    _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
        if (result) {
            backupSettingsToFile();
        }
    });
}

void SystemPage::restoreSettings() {
    if (fs::exists(Settings::Filename)) {
        _manager.pages().confirmation.show("ARE YOU SURE?", [this] (bool result) {
            if (result) {
                restoreSettingsFromFile();
            }
        });
    }
}

void SystemPage::saveSettingsToFlash() {
    _engine.suspend();
    _manager.pages().busy.show("SAVING SETTINGS ...");

    FileManager::task([this] () {
        _model.settings().writeToFlash();
        return fs::OK;
    }, [this] (fs::Error result) {
        showMessage("SETTINGS SAVED");
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.resume();
    });
}

void SystemPage::backupSettingsToFile() {
    _engine.suspend();
    _manager.pages().busy.show("BACKING UP SETTINGS ...");

    FileManager::task([this] () {
        return FileManager::writeSettings(_model.settings(), Settings::Filename);
    }, [this] (fs::Error result) {
        if (result == fs::OK) {
            showMessage("SETTINGS BACKED UP");
        } else {
            showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
        }
        // TODO lock ui mutex
        _manager.pages().busy.close();
        _engine.resume();
    });
}

void SystemPage::restoreSettingsFromFile() {
    _engine.suspend();
    _manager.pages().busy.show("RESTORING SETTINGS ...");

    FileManager::task([this] () {
        return FileManager::readSettings(_model.settings(), Settings::Filename);
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
        _engine.resume();
    });
}

void SystemPage::formatSdCard() {
    if (!FileManager::volumeAvailable()) {
        showMessage("NO SD CARD DETECTED!");
        return;
    }

    _manager.pages().confirmation.show("DO YOU REALLY WANT TO FORMAT THE SD CARD?", [this] (bool result) {
        if (result) {
            _manager.pages().busy.show("FORMATTING SD CARD ...");

            FileManager::task([] () {
                return FileManager::format();
            }, [this] (fs::Error result) {
                if (result == fs::OK) {
                    showMessage("SD CARD FORMATTED");
                } else {
                    showMessage(FixedStringBuilder<32>("FAILED (%s)", fs::errorToString(result)));
                }
                // TODO lock ui mutex
                _manager.pages().busy.close();
            });
        }
    });
}
