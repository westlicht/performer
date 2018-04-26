#pragma once

#include "Config.h"
#include "Types.h"
#include "NoteSequence.h"
#include "Serialize.h"

class NoteTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<NoteSequence, CONFIG_PATTERN_COUNT> NoteSequenceArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // playMode

    Types::PlayMode playMode() const { return _playMode; }
    void setPlayMode(Types::PlayMode playMode) {
        _playMode = ModelUtils::clampedEnum(playMode);
    }

    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    void printPlayMode(StringBuilder &str) const {
        str(Types::playModeName(playMode()));
    }

    // fillMode

    Types::FillMode fillMode() const { return _fillMode; }
    void setFillMode(Types::FillMode fillMode) {
        _fillMode = ModelUtils::clampedEnum(fillMode);
    }

    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    void printFillMode(StringBuilder &str) const {
        str(Types::fillModeName(fillMode()));
    }

    // midiMode (note, velocity, pitchbend, cc)

    // midiPort (midi, usb)

    // midiChannel (1-16)

    // slideTime

    int slideTime() const { return _slideTime; }
    void setSlideTime(int slideTime) {
        _slideTime = clamp(slideTime, 0, 100);
    }

    void editSlideTime(int value, bool shift) {
        setSlideTime(ModelUtils::adjustedByStep(slideTime(), value, 5, !shift));
    }

    void printSlideTime(StringBuilder &str) const {
        str("%d%%", slideTime());
    }

    // octave

    int octave() const { return _octave; }
    void setOctave(int octave) {
        _octave = clamp(octave, -10, 10);
    }

    void editOctave(int value, bool shift) {
        setOctave(octave() + value);
    }

    void printOctave(StringBuilder &str) const {
        str("%+d", octave());
    }

    // transpose

    int transpose() const { return _transpose; }
    void setTranspose(int transpose) {
        _transpose = clamp(transpose, -100, 100);
    }

    void editTranspose(int value, bool shift) {
        setTranspose(transpose() + value);
    }

    void printTranspose(StringBuilder &str) const {
        str("%+d", transpose());
    }

    // rotate

    int rotate() const { return _rotate; }
    void setRotate(int rotate) {
        _rotate = clamp(rotate, -64, 64);
    }

    void editRotate(int value, bool shift) {
        setRotate(rotate() + value);
    }

    void printRotate(StringBuilder &str) const {
        str("%+d", rotate());
    }

    // stepProbabilityBias

    int stepProbabilityBias() const { return _stepProbabilityBias; }
    void setStepProbabilityBias(int stepProbabilityBias) {
        _stepProbabilityBias = clamp(stepProbabilityBias, -NoteSequence::GateProbability::Range, NoteSequence::GateProbability::Range);
    }

    void editStepProbabilityBias(int value, bool shift) {
        setStepProbabilityBias(stepProbabilityBias() + value);
    }

    void printStepProbabilityBias(StringBuilder &str) const {
        str("%+.1f%%", stepProbabilityBias() * 12.5f);
    }

    // stepLengthBias

    int stepLengthBias() const { return _stepLengthBias; }
    void setStepLengthBias(int stepLengthBias) {
        _stepLengthBias = clamp(stepLengthBias, -NoteSequence::Length::Range, NoteSequence::Length::Range);
    }

    void editStepLengthBias(int value, bool shift) {
        setStepLengthBias(stepLengthBias() + value);
    }

    void printStepLengthBias(StringBuilder &str) const {
        str("%+.1f%%", stepLengthBias() * 12.5f);
    }

    // retriggerProbability
    // lengthVariationProbability

    // sequences

    const NoteSequenceArray &sequences() const { return _sequences; }
          NoteSequenceArray &sequences()       { return _sequences; }

    const NoteSequence &sequence(int index) const { return _sequences[index]; }
          NoteSequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    NoteTrack() { clear(); }

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Types::PlayMode _playMode;
    Types::FillMode _fillMode;
    uint8_t _slideTime;
    int8_t _octave;
    int8_t _transpose;
    int8_t _rotate;
    int8_t _stepProbabilityBias;
    int8_t _stepLengthBias;

    NoteSequenceArray _sequences;
};
