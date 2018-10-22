#include "LaunchpadController.h"

#include "core/Debug.h"

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

// Global
BUTTON(Navigate, LaunchpadDevice::FunctionRow, 0)
BUTTON(Left, LaunchpadDevice::FunctionRow, 1)
BUTTON(Right, LaunchpadDevice::FunctionRow, 2)
BUTTON(Up, LaunchpadDevice::FunctionRow, 3)
BUTTON(Down, LaunchpadDevice::FunctionRow, 4)
BUTTON(Play, LaunchpadDevice::FunctionRow, 6)
BUTTON(Shift, LaunchpadDevice::FunctionRow, 7)

// Sequence
BUTTON(Layer, LaunchpadDevice::FunctionRow, 1)
BUTTON(FirstStep, LaunchpadDevice::FunctionRow, 2)
BUTTON(LastStep, LaunchpadDevice::FunctionRow, 3)
BUTTON(Fill, LaunchpadDevice::FunctionRow, 6)

// Pattern
BUTTON(Latch, LaunchpadDevice::FunctionRow, 1)
BUTTON(Sync, LaunchpadDevice::FunctionRow, 2)

// Colors
static const LaunchpadDevice::Color Off(0, 0);
static const LaunchpadDevice::Color Red(1, 0);
static const LaunchpadDevice::Color Green(0, 1);
static const LaunchpadDevice::Color Yellow(1, 1);

struct LayerMapItem {
    uint8_t row;
    uint8_t col;
};

static const LayerMapItem noteSequenceLayerMap[] = {
    [int(NoteSequence::Layer::Gate)]                        =  { 0, 0 },
    [int(NoteSequence::Layer::GateProbability)]             =  { 1, 0 },
    [int(NoteSequence::Layer::Retrigger)]                   =  { 0, 1 },
    [int(NoteSequence::Layer::RetriggerProbability)]        =  { 1, 1 },
    [int(NoteSequence::Layer::Length)]                      =  { 0, 2 },
    [int(NoteSequence::Layer::LengthVariationRange)]        =  { 1, 2 },
    [int(NoteSequence::Layer::LengthVariationProbability)]  =  { 2, 2 },
    [int(NoteSequence::Layer::Note)]                        =  { 0, 3 },
    [int(NoteSequence::Layer::NoteVariationRange)]          =  { 1, 3 },
    [int(NoteSequence::Layer::NoteVariationProbability)]    =  { 2, 3 },
    [int(NoteSequence::Layer::Slide)]                       =  { 2, 0 },
};

static constexpr int noteSequenceLayerMapSize = sizeof(noteSequenceLayerMap) / sizeof(noteSequenceLayerMap[0]);

static const LayerMapItem curveSequenceLayerMap[] = {
    [int(CurveSequence::Layer::Shape)]                      =  { 0, 0 },
    [int(CurveSequence::Layer::Min)]                        =  { 0, 1 },
    [int(CurveSequence::Layer::Max)]                        =  { 0, 2 },
};

static constexpr int curveSequenceLayerMapSize = sizeof(curveSequenceLayerMap) / sizeof(curveSequenceLayerMap[0]);


LaunchpadController::LaunchpadController(ControllerManager &manager, Model &model, Engine &engine) :
    Controller(manager, model, engine),
    _project(model.project())
{
    _device.setSendMidiHandler([this] (const MidiMessage &message) {
        return sendMidi(message);
    });

    _device.setButtonHandler([this] (int row, int col, bool state) {
        // DBG("button %d/%d - %d", row, col, state);
        if (state) {
            buttonDown(row, col);
        } else {
            buttonUp(row, col);
        }
    });

    setMode(Mode::Sequence);
}

void LaunchpadController::update() {
    _device.clearLeds();

    CALL_MODE_FUNCTION(_mode, Draw)

    globalDraw();

    _device.syncLeds();
}

void LaunchpadController::recvMidi(const MidiMessage &message) {
    _device.recvMidi(message);
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
            setFunctionLed(col, selected ? Yellow : Off);
        }
        mirrorButton<Play>();
    }
}

bool LaunchpadController::globalButtonDown(const Button &button) {
    if (buttonState<Shift>() && button.isFunction()) {
        switch (button.function()) {
        case 0:
            setMode(Mode::Sequence);
            break;
        case 1:
            setMode(Mode::Pattern);
            break;
        case 2:
            setMode(Mode::Performer);
            break;
        case 6:
            _engine.togglePlay();
            break;
        }
        return true;
    }
    return false;
}

bool LaunchpadController::globalButtonUp(const Button &button) {
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
    } else {
        mirrorButton<Fill>();
        sequenceDrawSequence();
    }
}

void LaunchpadController::sequenceButtonDown(const Button &button) {
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
}

void LaunchpadController::sequenceButtonUp(const Button &button) {
    if (buttonState<Fill>()) {
        if (button.isScene()) {
            _project.playState().fillTrack(button.scene(), false);
        }
    }
    if (button.is<Fill>()) {
        _project.playState().fillAll(false);
    }
}

void LaunchpadController::sequenceUpdateNavigation() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note: {
        auto range = NoteSequence::layerRange(_project.selectedNoteSequenceLayer());
        _sequence.navigation.top = range.max / 8;
        _sequence.navigation.bottom = (range.min - 7) / 8;
        break;
    }
    case Track::TrackMode::Curve: {
        auto range = CurveSequence::layerRange(_project.selectedCurveSequenceLayer());
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
    default:
        sequence.step(linearIndex).setLayerValue(layer, value);
        break;
    }
}

void LaunchpadController::sequenceEditCurveStep(int row, int col) {
    auto &sequence = _project.selectedCurveSequence();
    auto layer = _project.selectedCurveSequenceLayer();

    int linearIndex = col + _sequence.navigation.col * 8;
    int value = (7 - row) + _sequence.navigation.row * 8;

    sequence.step(linearIndex).setLayerValue(layer, value);
}

void LaunchpadController::sequenceDrawLayer() {
    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        for (int i = 0; i < noteSequenceLayerMapSize; ++i) {
            const auto &item = noteSequenceLayerMap[i];
            bool selected = i == int(_project.selectedNoteSequenceLayer());
            setGridLed(item.row, item.col, selected ? Yellow : Green);
        }
        break;
    case Track::TrackMode::Curve:
        for (int i = 0; i < curveSequenceLayerMapSize; ++i) {
            const auto &item = curveSequenceLayerMap[i];
            bool selected = i == int(_project.selectedCurveSequenceLayer());
            setGridLed(item.row, item.col, selected ? Yellow : Green);
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
        drawStepRange(sequence.firstStep(), sequence.lastStep(), highlight == 0 ? sequence.firstStep() : sequence.lastStep());
        break;
    }
    case Track::TrackMode::Curve: {
        const auto &sequence = _project.selectedCurveSequence();
        drawStepRange(sequence.firstStep(), sequence.lastStep(), highlight == 0 ? sequence.firstStep() : sequence.lastStep());
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
        drawNoteSequenceGates(sequence, currentStep);
        break;
    case NoteSequence::Layer::Note:
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
    case CurveSequence::Layer::Min:
    case CurveSequence::Layer::Max:
        drawCurveSequenceDots(sequence, layer, currentStep);
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

    if (buttonState<Navigate>()) {
        navigationDraw(_pattern.navigation);
    } else {
        // selected patterns
        for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
            int pattern = playState.trackState(trackIndex).pattern();
            int requestedPattern = playState.trackState(trackIndex).requestedPattern();
            setGridLed(pattern + _pattern.navigation.row * 8, trackIndex, Green);
            if (pattern != requestedPattern) {
                setGridLed(requestedPattern + _pattern.navigation.row * 8, trackIndex, Yellow);
            }
        }
    }
}

void LaunchpadController::patternButtonDown(const Button &button) {
    auto &playState = _project.playState();

    if (buttonState<Navigate>()) {
        navigationButtonDown(_pattern.navigation, button);
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
}

void LaunchpadController::patternButtonUp(const Button &button) {
    if (button.is<Latch>()) {
        _project.playState().commitLatchedRequests();
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

void LaunchpadController::performerButtonDown(const Button &button) {
}

void LaunchpadController::performerButtonUp(const Button &button) {
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
            setGridLed(3 - row, col, selected ? Yellow : Green);
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
            Color(
                (mutedActivity || (selected && !unmutedActivity)) ? 1 : 0,
                (unmutedActivity || (selected && !mutedActivity)) ? 1 : 0
            )
        );
    }
}

void LaunchpadController::drawTracksGateAndMute(const Engine &engine, const PlayState &playState) {
    for (int track = 0; track < 8; ++track) {
        const auto &trackEngine = engine.trackEngine(track);
        setSceneLed(
            track,
            Color(
                trackEngine.mute(),
                trackEngine.activity()
            )
        );
    }
}

void LaunchpadController::drawStepRange(int first, int last, int highlit) {
    for (int i = first; i <= last; ++i) {
        setGridLed(i, i == highlit ? Yellow : Green);
    }
}

LaunchpadController::Color LaunchpadController::stepColor(bool active, bool current) const {
    // active current color   red green
    // 0      0       yellow  1   1
    // 0      1       red     1   0
    // 1      0       green   0   1
    // 1      1       red     1   0
    int red = (!active || current) ? 1 : 0;
    int green = current ? 0 : 1;
    return Color(red, green);
}

void LaunchpadController::drawNoteSequenceGates(const NoteSequence &sequence, int currentStep) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int stepIndex = row * 8 + col;
            const auto &step = sequence.step(stepIndex);
            setGridLed(row, col, Color(stepIndex == currentStep, step.gate() ? 1 : 0));
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
        setGridLed((7 - step.layerValue(layer)) + ofs, col, stepColor(step.gate(), stepIndex == currentStep));
    }
}

void LaunchpadController::drawCurveSequenceDots(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep) {
    int ofs = _sequence.navigation.row * 8;
    for (int col = 0; col < 8; ++col) {
        int stepIndex = col + _sequence.navigation.col * 8;
        const auto &step = sequence.step(stepIndex);
        setGridLed((7 - step.layerValue(layer)) + ofs, col, stepColor(true, stepIndex == currentStep));
    }
}

void LaunchpadController::drawBar(int col, int value, bool active, bool current) {
    int ofs = _sequence.navigation.row * 8;
    if (value >= 0) {
        for (int i = 0; i <= value; ++i) {
            setGridLed((7 - i) + ofs, col, Yellow);
        }
    } else {
        for (int i = 0; i >= value; --i) {
            setGridLed((7 - i) + ofs, col, Yellow);
        }
    }
    setGridLed((7 - value) + ofs, col, stepColor(active, current));
}

//----------------------------------------
// Led handling
//----------------------------------------

void LaunchpadController::setGridLed(int row, int col, Color color) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        _device.setLed(row, col, color);
    }
}

void LaunchpadController::setGridLed(int index, Color color) {
    if (index >= 0 && index < 64) {
        _device.setLed(index / 8, index % 8, color);
    }
}

void LaunchpadController::setFunctionLed(int col, Color color) {
    if (col >= 0 && col < 8) {
        _device.setLed(LaunchpadDevice::FunctionRow, col, color);
    }
}

void LaunchpadController::setSceneLed(int col, Color color) {
    if (col >= 0 && col < 8) {
        _device.setLed(LaunchpadDevice::SceneRow, col, color);
    }
}

//----------------------------------------
// Button handling
//----------------------------------------

void LaunchpadController::buttonDown(int row, int col) {
    Button button(row, col);

    if (globalButtonDown(button)) {
        return;
    }

    CALL_MODE_FUNCTION(_mode, ButtonDown, button);
}

void LaunchpadController::buttonUp(int row, int col) {
    Button button(row, col);

    if (globalButtonUp(button)) {
        return;
    }

    CALL_MODE_FUNCTION(_mode, ButtonUp, button);
}

bool LaunchpadController::buttonState(int row, int col) const {
    return _device.buttonState(row, col);
}
