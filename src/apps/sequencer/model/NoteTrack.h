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

    typedef std::array<NoteSequence, CONFIG_PATTERN_COUNT + CONFIG_SNAPSHOT_COUNT> NoteSequenceArray;

    enum class CvUpdateMode : uint8_t {
        Gate,
        Always,
        Last
    };

    static const char *cvUpdateModeName(CvUpdateMode mode) {
        switch (mode) {
        case CvUpdateMode::Gate:    return "Gate";
        case CvUpdateMode::Always:  return "Always";
        case CvUpdateMode::Last:    break;
        }
        return nullptr;
    }

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

    // cvUpdateMode

    CvUpdateMode cvUpdateMode() const { return _cvUpdateMode; }
    void setCvUpdateMode(CvUpdateMode cvUpdateMode) {
        _cvUpdateMode = ModelUtils::clampedEnum(cvUpdateMode);
    }

    void editCvUpdateMode(int value, bool shift) {
        setCvUpdateMode(ModelUtils::adjustedEnum(cvUpdateMode(), value));
    }

    void printCvUpdateMode(StringBuilder &str) const {
        str(cvUpdateModeName(cvUpdateMode()));
    }

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

    // gateProbabilityBias

    int gateProbabilityBias() const { return _gateProbabilityBias; }
    void setGateProbabilityBias(int gateProbabilityBias) {
        _gateProbabilityBias = clamp(gateProbabilityBias, -NoteSequence::GateProbability::Range, NoteSequence::GateProbability::Range);
    }

    void editGateProbabilityBias(int value, bool shift) {
        setGateProbabilityBias(gateProbabilityBias() + value);
    }

    void printGateProbabilityBias(StringBuilder &str) const {
        str("%+.1f%%", gateProbabilityBias() * 12.5f);
    }

    // retriggerProbabilityBias

    int retriggerProbabilityBias() const { return _retriggerProbabilityBias; }
    void setRetriggerProbabilityBias(int retriggerProbabilityBias) {
        _retriggerProbabilityBias = clamp(retriggerProbabilityBias, -NoteSequence::RetriggerProbability::Range, NoteSequence::RetriggerProbability::Range);
    }

    void editRetriggerProbabilityBias(int value, bool shift) {
        setRetriggerProbabilityBias(retriggerProbabilityBias() + value);
    }

    void printRetriggerProbabilityBias(StringBuilder &str) const {
        str("%+.1f%%", retriggerProbabilityBias() * 12.5f);
    }

    // lengthBias

    int lengthBias() const { return _lengthBias; }
    void setLengthBias(int lengthBias) {
        _lengthBias = clamp(lengthBias, -NoteSequence::Length::Range, NoteSequence::Length::Range);
    }

    void editLengthBias(int value, bool shift) {
        setLengthBias(lengthBias() + value);
    }

    void printLengthBias(StringBuilder &str) const {
        str("%+.1f%%", lengthBias() * 12.5f);
    }

    // noteProbabilityBias

    int noteProbabilityBias() const { return _noteProbabilityBias; }
    void setNoteProbabilityBias(int noteProbabilityBias) {
        _noteProbabilityBias = clamp(noteProbabilityBias, -NoteSequence::NoteVariationProbability::Range, NoteSequence::NoteVariationProbability::Range);
    }

    void editNoteProbabilityBias(int value, bool shift) {
        setNoteProbabilityBias(noteProbabilityBias() + value);
    }

    void printNoteProbabilityBias(StringBuilder &str) const {
        str("%+.1f%%", noteProbabilityBias() * 12.5f);
    }

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
    CvUpdateMode _cvUpdateMode;
    uint8_t _slideTime;
    int8_t _octave;
    int8_t _transpose;
    int8_t _rotate;
    int8_t _gateProbabilityBias;
    int8_t _retriggerProbabilityBias;
    int8_t _lengthBias;
    int8_t _noteProbabilityBias;

    NoteSequenceArray _sequences;
};
