#pragma once

#include "LaunchpadDevice.h"

#include "ui/Controller.h"

class LaunchpadController : public Controller {
public:
    LaunchpadController(ControllerManager &manager, Model &model, Engine &engine);

    virtual void update() override;

    virtual void recvMidi(const MidiMessage &message) override;

private:
    using Color = LaunchpadDevice::Color;

    struct Button {
        int row;
        int col;

        Button(int row, int col) : row(row), col(col) {}

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
    bool globalButtonDown(const Button &button);
    bool globalButtonUp(const Button &button);

    // Sequence mode
    void sequenceEnter();
    void sequenceExit();
    void sequenceDraw();
    void sequenceButtonDown(const Button &button);
    void sequenceButtonUp(const Button &button);

    void sequenceUpdateNavigation();

    void sequenceSetLayer(int row, int col);
    void sequenceSetFirstStep(int step);
    void sequenceSetLastStep(int step);
    void sequenceSetRunMode(int mode);
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
    void patternButtonDown(const Button &button);
    void patternButtonUp(const Button &button);

    // Performer mode
    void performerEnter();
    void performerExit();
    void performerDraw();
    void performerButtonDown(const Button &button);
    void performerButtonUp(const Button &button);

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
    void drawNoteSequenceNotes(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    void drawCurveSequenceDots(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep);
    void drawBar(int row, int value, bool active, bool current);

    // Led handling
    void setGridLed(int row, int col, Color color);
    void setGridLed(int index, Color color);
    void setFunctionLed(int col, Color color);
    void setSceneLed(int col, Color color);

    template<typename T>
    void setButtonLed(Color color) {
        _device.setLed(T::row, T::col, color);
    }

    template<typename T>
    void mirrorButton() {
        auto color = buttonState(T::row, T::col) ? Color(1, 1) : Color(0, 0);
        setButtonLed<T>(color);
    }

    // Button handling
    void buttonDown(int row, int col);
    void buttonUp(int row, int col);
    bool buttonState(int row, int col) const;

    template<typename T>
    bool buttonState() const {
        return buttonState(T::row, T::col);
    }

    Project &_project;
    LaunchpadDevice _device;
    Mode _mode = Mode::Sequence;

    struct {
        Navigation navigation = { 0, 0, 0, 7, 0, 0 };
    } _sequence;

    struct {
        Navigation navigation = { 0, 0, 0, 0, -1, 0 };
    } _pattern;
};
