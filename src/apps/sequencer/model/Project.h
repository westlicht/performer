#pragma once

#include "Config.h"
#include "Pattern.h"
#include "Serialize.h"

class Project {
public:
    // Patterns

    typedef std::array<Pattern, CONFIG_PATTERN_COUNT> PatternArray;

    const PatternArray &patterns() const { return _patterns; }
          PatternArray &patterns()       { return _patterns; }

    const Pattern &pattern(int index) const { return _patterns[index]; }
          Pattern &pattern(int index)       { return _patterns[index]; }

    // Parameters

    float bpm() const { return _bpm; }
    void setBpm(float bpm) { _bpm = bpm; }

    uint8_t swing() const { return _swing; }
    void setSwing(uint8_t swing) { _swing = swing; }

    // Pattern selection

    int selectedPatternIndex() const { return _selectedPatternIndex; }
    void setSelectedPatternIndex(int index) { _selectedPatternIndex = index; }

    bool isSelectedPattern(int index) const { return _selectedPatternIndex == index; }

    const Pattern &selectedPattern() const { return _patterns[_selectedPatternIndex]; }
          Pattern &selectedPattern()       { return _patterns[_selectedPatternIndex]; }

    // Track selection

    int selectedTrackIndex() const { return _selectedTrackIndex; }
    void setSelectedTrackIndex(int index) { _selectedTrackIndex = index; }

    bool isSelectedTrack(int index) const { return _selectedTrackIndex == index; }

    const Sequence &selectedSequence() const { return selectedPattern().sequence(_selectedTrackIndex); }
          Sequence &selectedSequence()       { return selectedPattern().sequence(_selectedTrackIndex); }

    // Serialization

    void write(ModelWriter &writer) const {
        writer.write(_bpm);
        writer.write(_swing);
        for (const auto &pattern : _patterns) {
            pattern.write(writer);
        }
    }

    void read(ModelReader &reader) {
        reader.read(_bpm);
        reader.read(_swing);
        for (auto &pattern : _patterns) {
            pattern.read(reader);
        }
    }

private:
    PatternArray _patterns;

    float _bpm = 120.f;
    uint8_t _swing = 0;

    int _selectedPatternIndex = 0;
    int _selectedTrackIndex = 0;
};
