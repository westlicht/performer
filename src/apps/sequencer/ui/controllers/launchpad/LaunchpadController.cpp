#include "LaunchpadController.h"

#include "core/Debug.h"
#include "os/os.h"

#define CALL_MODE_FUNCTION(_mode_, _function_, ...)                         \
    switch (_mode_) {                                                       \
    case Mode::Sequence:    sequence##_function_(__VA_ARGS__);      break;  \
    case Mode::Pattern:     pattern##_function_(__VA_ARGS__);       break;  \
    case Mode::Performer:   performer##_function_(__VA_ARGS__);     break;  \
    }

#define BUTTON(_name_, _row_, _col_)        \
    struct _name_ {                         \
        static constexpr int row = _row_;   \
        static constexpr int col = _col_;   \
    };

// Global buttons
BUTTON(Navigate, LaunchpadDevice::FunctionRow, 0)
BUTTON(Left, LaunchpadDevice::FunctionRow, 1)
BUTTON(Right, LaunchpadDevice::FunctionRow, 2)
BUTTON(Up, LaunchpadDevice::FunctionRow, 3)
BUTTON(Down, LaunchpadDevice::FunctionRow, 4)
BUTTON(Play, LaunchpadDevice::FunctionRow, 6)
BUTTON(Shift, LaunchpadDevice::FunctionRow, 7)

// Sequence page buttons
BUTTON(Layer, LaunchpadDevice::FunctionRow, 1)
BUTTON(FirstStep, LaunchpadDevice::FunctionRow, 2)
BUTTON(LastStep, LaunchpadDevice::FunctionRow, 3)
BUTTON(RunMode, LaunchpadDevice::FunctionRow, 4)

// Pattern page buttons
BUTTON(Latch, LaunchpadDevice::FunctionRow, 1)
BUTTON(Sync, LaunchpadDevice::FunctionRow, 2)

// Sequence and pattern page buttons
BUTTON(Fill, LaunchpadDevice::FunctionRow, 6)

struct LayerMapItem {
    uint8_t row;
    uint8_t col;
};

static const LayerMapItem noteSequenceLayerMap[] = {
    [int(NoteSequence::Layer::Gate)]                        =  { 0, 0 },
    [int(NoteSequence::Layer::GateProbability)]             =  { 1, 0 },
    [int(NoteSequence::Layer::GateOffset)]                  =  { 2, 0 },
    [int(NoteSequence::Layer::Slide)]                       =  { 3, 0 },
    [int(NoteSequence::Layer::Retrigger)]                   =  { 0, 1 },
    [int(NoteSequence::Layer::RetriggerProbability)]        =  { 1, 1 },
    [int(NoteSequence::Layer::Length)]                      =  { 0, 2 },
    [int(NoteSequence::Layer::LengthVariationRange)]        =  { 1, 2 },
    [int(NoteSequence::Layer::LengthVariationProbability)]  =  { 2, 2 },
    [int(NoteSequence::Layer::Note)]                        =  { 0, 3 },
    [int(NoteSequence::Layer::NoteVariationRange)]          =  { 1, 3 },
    [int(NoteSequence::Layer::NoteVariationProbability)]    =  { 2, 3 },
    [int(NoteSequence::Layer::Condition)]                   =  { 0, 4 },
};

static constexpr int noteSequenceLayerMapSize = sizeof(noteSequenceLayerMap) / sizeof(noteSequenceLayerMap[0]);

static const LayerMapItem curveSequenceLayerMap[] = {
    [int(CurveSequence::Layer::Shape)]                      =  { 0, 0 },
    [int(CurveSequence::Layer::ShapeVariation)]             =  { 1, 0 },
    [int(CurveSequence::Layer::ShapeVariationProbability)]  =  { 2, 0 },
    [int(CurveSequence::Layer::Min)]                        =  { 0, 1 },
    [int(CurveSequence::Layer::Max)]                        =  { 0, 2 },
    [int(CurveSequence::Layer::Gate)]                       =  { 0, 3 },
    [int(CurveSequence::Layer::GateProbability)]            =  { 1, 3 },
};

static constexpr int curveSequenceLayerMapSize = sizeof(curveSequenceLayerMap) / sizeof(curveSequenceLayerMap[0]);

struct RangeMap {
    int16_t min[2];
    int16_t max[2];
    int map(int value) const {
        return min[1] + ((value - min[0]) * (max[1] - min[1]) + (max[0] - min[0]) / 2) / (max[0] - min[0]);
    }
    int unmap(int value) const {
        return min[0] + ((value - min[1]) * (max[0] - min[0]) + (max[1] - min[1]) / 2) / (max[1] - min[1]);
    }
};

static const RangeMap curveMinMaxRangeMap = { { 0, 0 }, { 255, 7 } };

static const RangeMap *curveSequenceLayerRangeMap[] = {
    [int(CurveSequence::Layer::Shape)]                      = nullptr,
    [int(CurveSequence::Layer::ShapeVariation)]             = nullptr,
    [int(CurveSequence::Layer::ShapeVariationProbability)]  = nullptr,
    [int(CurveSequence::Layer::Min)]                        = &curveMinMaxRangeMap,
    [int(CurveSequence::Layer::Max)]                        = &curveMinMaxRangeMap,
    [int(CurveSequence::Layer::Gate)]                       = nullptr,
    [int(CurveSequence::Layer::GateProbability)]            = nullptr,
};

LaunchpadController::LaunchpadController(ControllerManager &manager, Model &model, Engine &engine, const ControllerInfo &info) :
    Controller(manager, model, engine),
    _project(model.project())
{
    if (info.productId == 0x0069) {
        // Launchpad Mini Mk2
        _device = _deviceContainer.create<LaunchpadMk2Device>();
    } else if (info.productId == 0x0051) {
        // Launchpad Pro
        _device = _deviceContainer.create<LaunchpadProDevice>();
    } else if (info.productId == 0x0113 || info.productId == 0x0104) {
        // Launchpad Mini Mk3 | Launchpad X
        _device = _deviceContainer.create<LaunchpadMk3Device>();
    } else if (info.productId == 0x0123) {
        // Launchpad Mk3 Pro
        _device = _deviceContainer.create<LaunchpadProMk3Device>();
    } else {
        _device = _deviceContainer.create<LaunchpadDevice>();
    }

    _device->setSendMidiHandler([this] (uint8_t cable, const MidiMessage &message) {
        return sendMidi(cable, message);
    });

    _device->setButtonHandler([this] (int row, int col, bool state) {
        // DBG("button %d/%d - %d", row, col, state);
        if (state) {
            buttonDown(row, col);
        } else {
            buttonUp(row, col);
        }
    });

    _device->initialize();

    setMode(Mode::Sequence);
}

LaunchpadController::~LaunchpadController() {
    _deviceContainer.destroy(_device);
}

void LaunchpadController::update() {
    _device->clearLeds();

    CALL_MODE_FUNCTION(_mode, Draw)

    globalDraw();

    _device->syncLeds();
}

void LaunchpadController::recvMidi(uint8_t cable, const MidiMessage &message) {
    _device->recvMidi(cable, message);
}

void LaunchpadController::setMode(Mode mode) {
    CALL_MODE_FUNCTION(_mode, Exit)
    _mode = mode;
    CALL_MODE_FUNCTION(_mode, Enter)
}

//----------------------------------------
// Global handlers
//----------------------------------------

void LaunchpadController::globalDraw() {
    if (buttonState<Shift>()) {
        for (int col = 0; col < 8; ++col) {
            bool selected = col == int(_mode) || col == 7;
            setFunctionLed(col, selected ? colorYellow() : colorOff());
        }
        mirrorButton<Play>();
    }
}

bool LaunchpadController::globalButton(const Button &button, ButtonAction action) {
    if (action == ButtonAction::Down) {
        if (buttonState<Shift>() && button.isFunction()) {
            switch (button.function()) {
            case 0:
                setMode(Mode::Sequence);
                break;
            case 1:
                setMode(Mode::Pattern);
                break;
            case 2:
                // TODO implement performer mode
                // setMode(Mode::Performer);
                break;
            case 6:
                _engine.togglePlay();
                break;
            }
            return true;
        }
    }
    return false;
}

//----------------------------------------
// Sequence mode
//----------------------------------------

void LaunchpadController::sequenceEnter() {
}

void LaunchpadController::sequenceExit() {
}

void LaunchpadController::sequenceDraw() {
    sequenceUpdateNavigation();

    mirrorButton<Navigate>();

    // selected track
    if (buttonState<Shift>()) {
        drawTracksGateAndMute(_engine, _project.playState());
        sequenceDrawSequence();
        return;
    } else {
        drawTracksGateAndSelected(_engine, _project.selectedTrackIndex());
    }

    if (buttonState<Navigate>()) {
        navigationDraw(_sequence.navigation);
    } else if (buttonState<Layer>()) {
        mirrorButton<Layer>();
        sequenceDrawLayer();
    } else if (buttonState<FirstStep>()) {
        mirrorButton<FirstStep>();
        sequenceDrawStepRange(0);
    } else if (buttonState<LastStep>()) {
        mirrorButton<LastStep>();
        sequenceDrawStepRange(1);
    } else if (buttonState<RunMode>()) {
        mirrorButton<RunMode>();
        sequenceDrawRunMode();
    } else {
        mirrorButton<Fill>();
        sequenceDrawSequence();
    }
}

void LaunchpadController::sequenceButton(const Button &button, ButtonAction action) {
    if (action == ButtonAction::Down) {
        if (buttonState<Shift>()) {
            if (button.isScene()) {
                _project.playState().toggleMuteTrack(button.scene());
            }
        } else if (buttonState<Navigate>()) {
            navigationButtonDown(_sequence.navigation, button);
        } else if (buttonState<Layer>()) {
            if (button.isGrid()) {
                sequenceSetLayer(button.row, button.col);
            }
        } else if (buttonState<FirstStep>()) {
            if (button.isGrid()) {
                sequenceSetFirstStep(button.gridIndex());
            }
        } else if (buttonState<LastStep>()) {
            if (button.isGrid()) {
                sequenceSetLastStep(button.gridIndex());
            }
        } else if (buttonState<RunMode>()) {
            if (button.isGrid()) {
                sequenceSetRunMode(button.gridIndex());
            }
        } else if (buttonState<Fill>()) {
            if (button.isScene()) {
                _project.playState().fillTrack(button.scene(), true);
            }
        } else {
            if (button.isGrid()) {
                sequenceEditStep(button.row, button.col);
            } else if (button.isScene()) {
                _project.setSelectedTrackIndex(button.scene());
            }
        }
    } else if (action == ButtonAction::Up) {
        if (buttonState<Fill>()) {
            if (button.isScene()) {
                _project.playState().fillTrack(button.scene(), false);
            }
        }
        if (button.is<Fill>()) {
            _project.playState().fillAll(false);
        }
    } else if (action == ButtonAction::DoublePress) {
        if (!buttonState<Shift>() &&
            !buttonState<Navigate>() &&
            !buttonState<Layer>() &&
            !buttonState<FirstStep>() &&
            !buttonState<LastStep>() &&
            !buttonState<RunMode>() &&
            !buttonState<Fill>() &&
            button.isGrid()) {
            // toggle gate
            sequenceToggleStep(button.row, button.col);
        }
    }
}

void LaunchpadController::sequenceUpdateNavigation() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note: {
        auto layer = _project.selectedNoteSequenceLayer();
        _sequence.navigation.left = 0;
        _sequence.navigation.right = layer == NoteSequence::Layer::Gate || layer == NoteSequence::Layer::Slide ? 0 : 7;

        auto range = NoteSequence::layerRange(_project.selectedNoteSequenceLayer());
        _sequence.navigation.top = range.max / 8;
        _sequence.navigation.bottom = (range.min - 7) / 8;

        break;
    }
    case Track::TrackMode::Curve: {
        _sequence.navigation.left = 0;
        _sequence.navigation.right = 7;

        auto range = CurveSequence::layerRange(_project.selectedCurveSequenceLayer());
        auto rangeMap = curveSequenceLayerRangeMap[int(_project.selectedCurveSequenceLayer())];
        if (rangeMap) {
            range.min = rangeMap->min[1];
            range.max = rangeMap->max[1];
        }
        _sequence.navigation.top = range.max / 8;
        _sequence.navigation.bottom = (range.min - 7) / 8;
        break;
    }
    default:
        break;
    }
}

void LaunchpadController::sequenceSetLayer(int row, int col) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note: {
        for (int i = 0; i < noteSequenceLayerMapSize; ++i) {
            const auto &item = noteSequenceLayerMap[i];
            if (row == item.row && col == item.col) {
                _project.setSelectedNoteSequenceLayer(NoteSequence::Layer(i));
                break;
            }
        }
        break;
    }
    case Track::TrackMode::Curve: {
        for (int i = 0; i < curveSequenceLayerMapSize; ++i) {
            const auto &item = curveSequenceLayerMap[i];
            if (row == item.row && col == item.col) {
                _project.setSelectedCurveSequenceLayer(CurveSequence::Layer(i));
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

void LaunchpadController::sequenceSetFirstStep(int step) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _project.selectedNoteSequence().setFirstStep(step);
        break;
    case Track::TrackMode::Curve:
        _project.selectedCurveSequence().setFirstStep(step);
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceSetLastStep(int step) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _project.selectedNoteSequence().setLastStep(step);
        break;
    case Track::TrackMode::Curve:
        _project.selectedCurveSequence().setLastStep(step);
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceSetRunMode(int mode) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        _project.selectedNoteSequence().setRunMode(Types::RunMode(mode));
        break;
    case Track::TrackMode::Curve:
        _project.selectedCurveSequence().setRunMode(Types::RunMode(mode));
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceToggleStep(int row, int col) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        sequenceToggleNoteStep(row, col);
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceToggleNoteStep(int row, int col) {
    auto &sequence = _project.selectedNoteSequence();
    auto layer = _project.selectedNoteSequenceLayer();

    int linearIndex = col + _sequence.navigation.col * 8;

    switch (layer) {
    case NoteSequence::Layer::Gate:
    case NoteSequence::Layer::Slide:
        break;
    default:
        sequence.step(linearIndex).toggleGate();
        break;
    }
}

void LaunchpadController::sequenceEditStep(int row, int col) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        sequenceEditNoteStep(row, col);
        break;
    case Track::TrackMode::Curve:
        sequenceEditCurveStep(row, col);
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceEditNoteStep(int row, int col) {
    auto &sequence = _project.selectedNoteSequence();
    auto layer = _project.selectedNoteSequenceLayer();

    int gridIndex = row * 8 + col;
    int linearIndex = col + _sequence.navigation.col * 8;
    int value = (7 - row) + _sequence.navigation.row * 8;

    switch (layer) {
    case NoteSequence::Layer::Gate:
        sequence.step(gridIndex).toggleGate();
        break;
    case NoteSequence::Layer::Slide:
        sequence.step(gridIndex).toggleSlide();
        break;
    default:
        sequence.step(linearIndex).setLayerValue(layer, value);
        break;
    }
}

void LaunchpadController::sequenceEditCurveStep(int row, int col) {
    auto &sequence = _project.selectedCurveSequence();
    auto layer = _project.selectedCurveSequenceLayer();
    auto rangeMap = curveSequenceLayerRangeMap[int(_project.selectedCurveSequenceLayer())];

    int linearIndex = col + _sequence.navigation.col * 8;
    int value = (7 - row) + _sequence.navigation.row * 8;
    if (rangeMap) {
        value = rangeMap->unmap(value);
    }

    sequence.step(linearIndex).setLayerValue(layer, value);
}

void LaunchpadController::sequenceDrawLayer() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        for (int i = 0; i < noteSequenceLayerMapSize; ++i) {
            const auto &item = noteSequenceLayerMap[i];
            bool selected = i == int(_project.selectedNoteSequenceLayer());
            setGridLed(item.row, item.col, selected ? colorYellow() : colorGreen());
        }
        break;
    case Track::TrackMode::Curve:
        for (int i = 0; i < curveSequenceLayerMapSize; ++i) {
            const auto &item = curveSequenceLayerMap[i];
            bool selected = i == int(_project.selectedCurveSequenceLayer());
            setGridLed(item.row, item.col, selected ? colorYellow() : colorGreen());
        }
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceDrawStepRange(int highlight) {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note: {
        const auto &sequence = _project.selectedNoteSequence();
        drawRange(sequence.firstStep(), sequence.lastStep(), highlight == 0 ? sequence.firstStep() : sequence.lastStep());
        break;
    }
    case Track::TrackMode::Curve: {
        const auto &sequence = _project.selectedCurveSequence();
        drawRange(sequence.firstStep(), sequence.lastStep(), highlight == 0 ? sequence.firstStep() : sequence.lastStep());
        break;
    }
    default:
        break;
    }
}

void LaunchpadController::sequenceDrawRunMode() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note: {
        drawEnum(_project.selectedNoteSequence().runMode());
        break;
    }
    case Track::TrackMode::Curve: {
        drawEnum(_project.selectedCurveSequence().runMode());
        break;
    }
    default:
        break;
    }
}

void LaunchpadController::sequenceDrawSequence() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        sequenceDrawNoteSequence();
        break;
    case Track::TrackMode::Curve:
        sequenceDrawCurveSequence();
        break;
    default:
        break;
    }
}

void LaunchpadController::sequenceDrawNoteSequence() {
    const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
    const auto &sequence = _project.selectedNoteSequence();
    auto layer = _project.selectedNoteSequenceLayer();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    switch (layer) {
    case NoteSequence::Layer::Gate:
    case NoteSequence::Layer::Slide:
        drawNoteSequenceBits(sequence, layer, currentStep);
        break;
    case NoteSequence::Layer::Note:
        drawNoteSequenceNotes(sequence, layer, currentStep);
        break;
    case NoteSequence::Layer::Condition:
        drawNoteSequenceDots(sequence, layer, currentStep);
        break;
    default:
        drawNoteSequenceBars(sequence, layer, currentStep);
        break;
    }
}

void LaunchpadController::sequenceDrawCurveSequence() {
    const auto &trackEngine = _engine.selectedTrackEngine().as<CurveTrackEngine>();
    const auto &sequence = _project.selectedCurveSequence();
    auto layer = _project.selectedCurveSequenceLayer();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    switch (layer) {
    case CurveSequence::Layer::Shape:
    case CurveSequence::Layer::ShapeVariation:
    case CurveSequence::Layer::Min:
    case CurveSequence::Layer::Max:
    case CurveSequence::Layer::Gate:
        drawCurveSequenceDots(sequence, layer, currentStep);
        break;
    case CurveSequence::Layer::ShapeVariationProbability:
    case CurveSequence::Layer::GateProbability:
        drawCurveSequenceBars(sequence, layer, currentStep);
        break;
    default:
        break;
    }
}

//----------------------------------------
// Pattern mode
//----------------------------------------

void LaunchpadController::patternEnter() {
}

void LaunchpadController::patternExit() {
    _project.playState().commitLatchedRequests();
}

void LaunchpadController::patternDraw() {
    const auto &playState = _project.playState();

    mirrorButton<Navigate>();
    mirrorButton<Latch>();
    mirrorButton<Sync>();


    if (buttonState<Shift>()) {
        drawTracksGateAndMute(_engine, _project.playState());
    } else {
        drawTracksGateAndSelected(_engine, _project.selectedTrackIndex());
    }

    if (buttonState<Navigate>()) {
        navigationDraw(_pattern.navigation);
    } else {
        for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
            // draw edited patterns (note tracks -> dim yellow, curve tracks -> dim red)
            for (int row = 0; row < 8; ++row) {
                int patternIndex = row - _pattern.navigation.row * 8;
                const auto &track = _project.track(trackIndex);

                switch (track.trackMode()) {
                case Track::TrackMode::Note:
                    if (track.noteTrack().sequence(patternIndex).isEdited()) {
                        setGridLed(row, trackIndex, colorYellow(1));
                    }
                    break;
                case Track::TrackMode::Curve:
                    if (track.curveTrack().sequence(patternIndex).isEdited()) {
                        setGridLed(row, trackIndex, colorRed(1));
                    }
                    break;
                default:
                    break;
                }
            }

            // draw selected (green) & requested (dim green) patterns
            int pattern = playState.trackState(trackIndex).pattern();
            int requestedPattern = playState.trackState(trackIndex).requestedPattern();
            setGridLed(pattern + _pattern.navigation.row * 8, trackIndex, colorGreen());
            if (pattern != requestedPattern) {
                setGridLed(requestedPattern + _pattern.navigation.row * 8, trackIndex, colorGreen(1));
            }
        }

        mirrorButton<Fill>();
    }
}

void LaunchpadController::patternButton(const Button &button, ButtonAction action) {
    auto &playState = _project.playState();

    if (action == ButtonAction::Down) {
        if (buttonState<Shift>()) {
            if (button.isScene()) {
                _project.playState().toggleMuteTrack(button.scene());
            }
        } else if (buttonState<Navigate>()) {
            navigationButtonDown(_pattern.navigation, button);
        } else if (buttonState<Fill>()) {
            if (button.isScene()) {
                _project.playState().fillTrack(button.scene(), true);
            }
        } else {
            PlayState::ExecuteType executeType = PlayState::ExecuteType::Immediate;
            if (buttonState<Latch>()) {
                executeType = PlayState::ExecuteType::Latched;
            } else if (buttonState<Sync>()) {
                executeType = PlayState::ExecuteType::Synced;
            }

            if (button.isScene()) {
                int pattern = button.scene() - _pattern.navigation.row * 8;
                playState.selectPattern(pattern, executeType);
            }

            if (button.isGrid()) {
                int pattern = button.row - _pattern.navigation.row * 8;
                int trackIndex = button.col;
                playState.selectTrackPattern(trackIndex, pattern, executeType);
            }
        }
    } else if (action == ButtonAction::Up) {
        if (buttonState<Fill>()) {
            if (button.isScene()) {
                playState.fillTrack(button.scene(), false);
            }
        }
        if (button.is<Fill>()) {
            playState.fillAll(false);
        } else if (button.is<Latch>()) {
            playState.commitLatchedRequests();
        }
    }

}

//----------------------------------------
// Performer mode
//----------------------------------------

void LaunchpadController::performerEnter() {
}

void LaunchpadController::performerExit() {
}

void LaunchpadController::performerDraw() {
}

void LaunchpadController::performerButton(const Button &button, ButtonAction action) {
}

//----------------------------------------
// Navigation
//----------------------------------------

void LaunchpadController::navigationDraw(const Navigation &navigation) {
    mirrorButton<Left>();
    mirrorButton<Right>();
    mirrorButton<Up>();
    mirrorButton<Down>();

    for (int row = navigation.bottom; row <= navigation.top; ++row) {
        for (int col = navigation.left; col <= navigation.right; ++col) {
            bool selected = row == navigation.row && col == navigation.col;
            setGridLed(3 - row, col, selected ? colorYellow() : colorGreen());
        }
    }
}

void LaunchpadController::navigationButtonDown(Navigation &navigation, const Button &button) {
    int col = button.is<Left>() ? -1 : button.is<Right>() ? 1 : 0;
    navigation.col = clamp(navigation.col + col, int(navigation.left), int(navigation.right));

    int row = button.is<Down>() ? -1 : button.is<Up>() ? 1 : 0;
    navigation.row = clamp(navigation.row + row, int(navigation.bottom), int(navigation.top));

    if (button.isGrid()) {
        int col = button.col;
        int row = 3 - button.row;
        if (col >= navigation.left && col <= navigation.right && row >= navigation.bottom && row <= navigation.top) {
            navigation.col = col;
            navigation.row = row;
        }
    }
}

//----------------------------------------
// Drawing
//----------------------------------------

void LaunchpadController::drawTracksGateAndSelected(const Engine &engine, int selectedTrack) {
    for (int track = 0; track < 8; ++track) {
        const auto &trackEngine = engine.trackEngine(track);
        bool unmutedActivity = trackEngine.activity() && !trackEngine.mute();
        bool mutedActivity = trackEngine.activity() && trackEngine.mute();
        bool selected = track == selectedTrack;
        setSceneLed(
            track,
            color(
                (mutedActivity || (selected && !unmutedActivity)),
                (unmutedActivity || (selected && !mutedActivity))
            )
        );
    }
}

void LaunchpadController::drawTracksGateAndMute(const Engine &engine, const PlayState &playState) {
    for (int track = 0; track < 8; ++track) {
        const auto &trackEngine = engine.trackEngine(track);
        setSceneLed(
            track,
            color(
                trackEngine.mute(),
                trackEngine.activity()
            )
        );
    }
}

void LaunchpadController::drawRange(int first, int last, int selected) {
    for (int i = first; i <= last; ++i) {
        setGridLed(i, i == selected ? colorYellow() : colorGreen());
    }
}

LaunchpadController::Color LaunchpadController::stepColor(bool active, bool current) const {
    // active current color   red green
    // 0      0       yellow  1   1
    // 0      1       red     1   0
    // 1      0       green   0   1
    // 1      1       red     1   0
    bool red = (!active || current);
    bool green = !current;
    return color(red, green);
}

void LaunchpadController::drawNoteSequenceBits(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int stepIndex = row * 8 + col;
            const auto &step = sequence.step(stepIndex);
            setGridLed(row, col, color(stepIndex == currentStep, step.layerValue(layer) != 0));
        }
    }
}

void LaunchpadController::drawNoteSequenceBars(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep) {
    for (int col = 0; col < 8; ++col) {
        int stepIndex = col + _sequence.navigation.col * 8;
        const auto &step = sequence.step(stepIndex);
        drawBar(col, step.layerValue(layer), step.gate(), stepIndex == currentStep);
    }
}

void LaunchpadController::drawNoteSequenceDots(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep) {
    int ofs = _sequence.navigation.row * 8;
    for (int col = 0; col < 8; ++col) {
        int stepIndex = col + _sequence.navigation.col * 8;
        const auto &step = sequence.step(stepIndex);
        int value = step.layerValue(layer);
        setGridLed((7 - value) + ofs, col, stepColor(true, stepIndex == currentStep));
    }
}

void LaunchpadController::drawNoteSequenceNotes(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep) {
    int ofs = _sequence.navigation.row * 8;

    // draw octave lines
    int octave = sequence.selectedScale(_project.scale()).notesPerOctave();
    for (int row = 0; row < 8; ++row) {
        if (modulo(row + ofs, octave) == 0) {
            for (int col = 0; col < 8; ++col) {
            setGridLed(7 - row, col, colorYellow(1));
            }
        }
    }

    // draw notes
    for (int col = 0; col < 8; ++col) {
        int stepIndex = col + _sequence.navigation.col * 8;
        const auto &step = sequence.step(stepIndex);
        setGridLed((7 - step.layerValue(layer)) + ofs, col, stepColor(step.gate(), stepIndex == currentStep));
    }
}

void LaunchpadController::drawCurveSequenceBars(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep) {
    for (int col = 0; col < 8; ++col) {
        int stepIndex = col + _sequence.navigation.col * 8;
        const auto &step = sequence.step(stepIndex);
        drawBar(col, step.layerValue(layer), true, stepIndex == currentStep);
    }
}

void LaunchpadController::drawCurveSequenceDots(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep) {
    auto rangeMap = curveSequenceLayerRangeMap[int(_project.selectedCurveSequenceLayer())];
    int ofs = _sequence.navigation.row * 8;
    for (int col = 0; col < 8; ++col) {
        int stepIndex = col + _sequence.navigation.col * 8;
        const auto &step = sequence.step(stepIndex);
        int value = step.layerValue(layer);
        if (rangeMap) {
            value = rangeMap->map(value);
        }
        setGridLed((7 - value) + ofs, col, stepColor(true, stepIndex == currentStep));
    }
}

void LaunchpadController::drawBar(int col, int value, bool active, bool current) {
    int ofs = _sequence.navigation.row * 8;
    if (value >= 0) {
        for (int i = 0; i <= value; ++i) {
            setGridLed((7 - i) + ofs, col, colorYellow());
        }
    } else {
        for (int i = 0; i >= value; --i) {
            setGridLed((7 - i) + ofs, col, colorYellow());
        }
    }
    setGridLed((7 - value) + ofs, col, stepColor(active, current));
}

//----------------------------------------
// Led handling
//----------------------------------------

void LaunchpadController::setGridLed(int row, int col, Color color) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        _device->setLed(row, col, color);
    }
}

void LaunchpadController::setGridLed(int index, Color color) {
    if (index >= 0 && index < 64) {
        _device->setLed(index / 8, index % 8, color);
    }
}

void LaunchpadController::setFunctionLed(int col, Color color) {
    if (col >= 0 && col < 8) {
        _device->setLed(LaunchpadDevice::FunctionRow, col, color);
    }
}

void LaunchpadController::setSceneLed(int col, Color color) {
    if (col >= 0 && col < 8) {
        _device->setLed(LaunchpadDevice::SceneRow, col, color);
    }
}

//----------------------------------------
// Button handling
//----------------------------------------

void LaunchpadController::dispatchButtonEvent(const Button& button, ButtonAction action) {
    if (globalButton(button, action)) {
        return;
    }

    CALL_MODE_FUNCTION(_mode, Button, button, action);
}

void LaunchpadController::buttonDown(int row, int col) {
    Button button(row, col);

    dispatchButtonEvent(button, ButtonAction::Down);

    uint32_t currentTicks = os::ticks();
    uint32_t deltaTicks = currentTicks - _buttonTracker.lastTicks;

    if (button != _buttonTracker.lastButton || deltaTicks > os::time::ms(300)) {
        _buttonTracker.count = 1;
    } else {
        ++_buttonTracker.count;
    }

    _buttonTracker.lastButton = button;
    _buttonTracker.lastTicks = currentTicks;

    if (_buttonTracker.count == 1) dispatchButtonEvent(button, ButtonAction::Press);
    if (_buttonTracker.count == 2) dispatchButtonEvent(button, ButtonAction::DoublePress);
}

void LaunchpadController::buttonUp(int row, int col) {
    dispatchButtonEvent(Button(row, col), ButtonAction::Up);
}

bool LaunchpadController::buttonState(int row, int col) const {
    return _device->buttonState(row, col);
}
