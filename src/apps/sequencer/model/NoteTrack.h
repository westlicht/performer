#pragma once

#include "Config.h"
#include "Types.h"
#include "NoteSequence.h"
#include "Serialize.h"
#include "Routing.h"

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

    int slideTime() const { return _slideTime.get(isRouted(Routing::Target::SlideTime)); }
    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

    void editSlideTime(int value, bool shift) {
        if (!isRouted(Routing::Target::SlideTime)) {
            setSlideTime(ModelUtils::adjustedByStep(slideTime(), value, 5, !shift));
        }
    }

    void printSlideTime(StringBuilder &str) const {
        _routed.print(str, Routing::Target::SlideTime);
        str("%d%%", slideTime());
    }

    // octave

    int octave() const { return _octave.get(isRouted(Routing::Target::Octave)); }
    void setOctave(int octave, bool routed = false) {
        _octave.set(clamp(octave, -10, 10), routed);
    }

    void editOctave(int value, bool shift) {
        if (!isRouted(Routing::Target::Octave)) {
            setOctave(octave() + value);
        }
    }

    void printOctave(StringBuilder &str) const {
        _routed.print(str, Routing::Target::Octave);
        str("%+d", octave());
    }

    // transpose

    int transpose() const { return _transpose.get(isRouted(Routing::Target::Transpose)); }
    void setTranspose(int transpose, bool routed = false) {
        _transpose.set(clamp(transpose, -100, 100), routed);
    }

    void editTranspose(int value, bool shift) {
        if (!isRouted(Routing::Target::Transpose)) {
            setTranspose(transpose() + value);
        }
    }

    void printTranspose(StringBuilder &str) const {
        _routed.print(str, Routing::Target::Transpose);
        str("%+d", transpose());
    }

    // rotate

    int rotate() const { return _rotate.get(isRouted(Routing::Target::Rotate)); }
    void setRotate(int rotate, bool routed = false) {
        _rotate.set(clamp(rotate, -64, 64), routed);
    }

    void editRotate(int value, bool shift) {
        if (!isRouted(Routing::Target::Rotate)) {
            setRotate(rotate() + value);
        }
    }

    void printRotate(StringBuilder &str) const {
        _routed.print(str, Routing::Target::Rotate);
        str("%+d", rotate());
    }

    // gateProbabilityBias

    int gateProbabilityBias() const { return _gateProbabilityBias.get(isRouted(Routing::Target::GateProbabilityBias)); }
    void setGateProbabilityBias(int gateProbabilityBias, bool routed = false) {
        _gateProbabilityBias.set(clamp(gateProbabilityBias, -NoteSequence::GateProbability::Range, NoteSequence::GateProbability::Range), routed);
    }

    void editGateProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::GateProbabilityBias)) {
            setGateProbabilityBias(gateProbabilityBias() + value);
        }
    }

    void printGateProbabilityBias(StringBuilder &str) const {
        _routed.print(str, Routing::Target::GateProbabilityBias);
        str("%+.1f%%", gateProbabilityBias() * 12.5f);
    }

    // retriggerProbabilityBias

    int retriggerProbabilityBias() const { return _retriggerProbabilityBias.get(isRouted(Routing::Target::RetriggerProbabilityBias)); }
    void setRetriggerProbabilityBias(int retriggerProbabilityBias, bool routed = false) {
        _retriggerProbabilityBias.set(clamp(retriggerProbabilityBias, -NoteSequence::RetriggerProbability::Range, NoteSequence::RetriggerProbability::Range), routed);
    }

    void editRetriggerProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::RetriggerProbabilityBias)) {
            setRetriggerProbabilityBias(retriggerProbabilityBias() + value);
        }
    }

    void printRetriggerProbabilityBias(StringBuilder &str) const {
        _routed.print(str, Routing::Target::RetriggerProbabilityBias);
        str("%+.1f%%", retriggerProbabilityBias() * 12.5f);
    }

    // lengthBias

    int lengthBias() const { return _lengthBias.get(isRouted(Routing::Target::LengthBias)); }
    void setLengthBias(int lengthBias, bool routed = false) {
        _lengthBias.set(clamp(lengthBias, -NoteSequence::Length::Range, NoteSequence::Length::Range), routed);
    }

    void editLengthBias(int value, bool shift) {
        if (!isRouted(Routing::Target::LengthBias)) {
            setLengthBias(lengthBias() + value);
        }
    }

    void printLengthBias(StringBuilder &str) const {
        _routed.print(str, Routing::Target::LengthBias);
        str("%+.1f%%", lengthBias() * 12.5f);
    }

    // noteProbabilityBias

    int noteProbabilityBias() const { return _noteProbabilityBias.get(isRouted(Routing::Target::NoteProbabilityBias)); }
    void setNoteProbabilityBias(int noteProbabilityBias, bool routed = false) {
        _noteProbabilityBias.set(clamp(noteProbabilityBias, -NoteSequence::NoteVariationProbability::Range, NoteSequence::NoteVariationProbability::Range), routed);
    }

    void editNoteProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::NoteProbabilityBias)) {
            setNoteProbabilityBias(noteProbabilityBias() + value);
        }
    }

    void printNoteProbabilityBias(StringBuilder &str) const {
        _routed.print(str, Routing::Target::NoteProbabilityBias);
        str("%+.1f%%", noteProbabilityBias() * 12.5f);
    }

    // sequences

    const NoteSequenceArray &sequences() const { return _sequences; }
          NoteSequenceArray &sequences()       { return _sequences; }

    const NoteSequence &sequence(int index) const { return _sequences[index]; }
          NoteSequence &sequence(int index)       { return _sequences[index]; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return _routed.has(target); }
    inline void setRouted(Routing::Target target, bool routed) { _routed.set(target, routed); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

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
    Routable<uint8_t> _slideTime;
    Routable<int8_t> _octave;
    Routable<int8_t> _transpose;
    Routable<int8_t> _rotate;
    Routable<int8_t> _gateProbabilityBias;
    Routable<int8_t> _retriggerProbabilityBias;
    Routable<int8_t> _lengthBias;
    Routable<int8_t> _noteProbabilityBias;

    RoutableSet<Routing::Target::TrackFirst, Routing::Target::TrackLast> _routed;

    NoteSequenceArray _sequences;
};
