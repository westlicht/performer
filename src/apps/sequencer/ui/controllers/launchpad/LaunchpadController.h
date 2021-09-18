#pragma once

#include "LaunchpadDevice.h"
#include "LaunchpadMk2Device.h"
#include "LaunchpadMk3Device.h"
#include "LaunchpadProDevice.h"
#include "LaunchpadProMk3Device.h"

#include "ui/Controller.h"

#include "core/utils/Container.h"

class LaunchpadController : public Controller {
public:
    LaunchpadController(ControllerManager &manager, Model &model, Engine &engine, const ControllerInfo &info);
    virtual ~LaunchpadController();

    virtual void update() override;

    virtual void recvMidi(uint8_t cable, const MidiMessage &message) override;

private:
    using Color = LaunchpadDevice::Color;

    inline Color color(bool red, bool green, int brightness = 3) const {
        return Color(red ? brightness : 0, green ? brightness : 0);
    }

    inline Color colorOff() const { return Color(0, 0); }
    inline Color colorRed(int brightness = 3) const { return Color(brightness, 0); }
    inline Color colorGreen(int brightness = 3) const { return Color(0, brightness); }
    inline Color colorYellow(int brightness = 3) const { return Color(brightness, brightness); }

    struct Button {
        int row = -1;
        int col = -1;

        Button() = default;
        Button(int row, int col) : row(row), col(col) {}

        bool operator==(const Button &other) const {
            return row == other.row && col == other.col;
        }

        bool operator!=(const Button &other) const {
            return !(*this == other);
        }

        template<typename T>
        bool is() const {
            return row == T::row && col == T::col;
        }

        bool isGrid() const { return row < 8; }
        bool isFunction() const { return row == LaunchpadDevice::FunctionRow; }
        bool isScene() const { return row == LaunchpadDevice::SceneRow; }

        int gridIndex() const { return row * 8 + col; }
        int function() const { return isFunction() ? col : -1; }
        int scene() const { return isScene() ? col : -1; }
    };

    enum class ButtonAction {
        Down,
        Up,
        Press,
        DoublePress
    };

    struct Navigation {
        int8_t col;
        int8_t row;
        int8_t left;
        int8_t right;
        int8_t bottom;
        int8_t top;
    };

    enum class Mode : uint8_t {
        Sequence,
        Pattern,
        Performer,
    };

    void setMode(Mode mode);

    // Global handlers
    void globalDraw();
    bool globalButton(const Button &button, ButtonAction action);

    // Sequence mode
    void sequenceEnter();
    void sequenceExit();
    void sequenceDraw();
    void sequenceButton(const Button &button, ButtonAction action);

    void sequenceUpdateNavigation();

    void sequenceSetLayer(int row, int col);
    void sequenceSetFirstStep(int step);
    void sequenceSetLastStep(int step);
    void sequenceSetRunMode(int mode);
    void sequenceToggleStep(int row, int col);
    void sequenceToggleNoteStep(int row, int col);
    void sequenceEditStep(int row, int col);
    void sequenceEditNoteStep(int row, int col);
    void sequenceEditCurveStep(int row, int col);

    void sequenceDrawLayer();
    void sequenceDrawStepRange(int highlight);
    void sequenceDrawRunMode();
    void sequenceDrawSequence();
    void sequenceDrawNoteSequence();
    void sequenceDrawCurveSequence();

    // Pattern mode
    void patternEnter();
    void patternExit();
    void patternDraw();
    void patternButton(const Button &button, ButtonAction action);

    // Performer mode
    void performerEnter();
    void performerExit();
    void performerDraw();
    void performerButton(const Button &button, ButtonAction action);

    // Navigation
    void navigationDraw(const Navigation &navigation);
    void navigationButtonDown(Navigation &navigation, const Button &button);

    // Drawing
    void drawTracksGateAndSelected(const Engine &engine, int selectedTrack);
    void drawTracksGateAndMute(const Engine &engine, const PlayState &playState);

    template<typename Enum>
    void drawEnum(Enum e) { drawRange(0, int(Enum::Last) - 1, int(e)); }
    void drawRange(int first, int last, int selected);

    Color stepColor(bool active, bool current) const;
    void drawNoteSequenceBits(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    void drawNoteSequenceBars(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    void drawNoteSequenceDots(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    void drawNoteSequenceNotes(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    void drawCurveSequenceBars(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep);
    void drawCurveSequenceDots(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep);
    void drawBar(int row, int value, bool active, bool current);

    // Led handling
    void setGridLed(int row, int col, Color color);
    void setGridLed(int index, Color color);
    void setFunctionLed(int col, Color color);
    void setSceneLed(int col, Color color);

    template<typename T>
    void setButtonLed(Color color) {
        _device->setLed(T::row, T::col, color);
    }

    template<typename T>
    void mirrorButton() {
        setButtonLed<T>(buttonState(T::row, T::col) ? color(true, true) : color(false, false));
    }

    // Button handling
    void dispatchButtonEvent(const Button& button, ButtonAction action);
    void buttonDown(int row, int col);
    void buttonUp(int row, int col);
    bool buttonState(int row, int col) const;

    template<typename T>
    bool buttonState() const {
        return buttonState(T::row, T::col);
    }

    struct {
        Button lastButton;
        uint32_t lastTicks = 0;
        uint8_t count = 1;
    } _buttonTracker;

    Project &_project;
    Container<LaunchpadDevice, LaunchpadMk2Device, LaunchpadMk3Device, LaunchpadProDevice, LaunchpadProMk3Device> _deviceContainer;
    LaunchpadDevice *_device;
    Mode _mode = Mode::Sequence;

    struct {
        Navigation navigation = { 0, 0, 0, 7, 0, 0 };
    } _sequence;

    struct {
        Navigation navigation = { 0, 0, 0, 0, -1, 0 };
    } _pattern;
};
