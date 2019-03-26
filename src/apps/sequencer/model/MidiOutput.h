#pragma once

#include "Config.h"

#include "Types.h"
#include "MidiConfig.h"
#include "Serialize.h"
#include "ModelUtils.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

class Project;
class NoteSequence;
class CurveSequence;

class MidiOutput {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    class Output {
    public:
        enum class Event : uint8_t {
            None,
            Note,
            ControlChange,
            Last,
        };

        static const char *eventName(Event event) {
            switch (event) {
            case Event::None:           return "None";
            case Event::Note:           return "Note";
            case Event::ControlChange:  return "CC";
            case Event::Last:           break;
            }
            return nullptr;
        }

        enum class GateSource : uint8_t {
            FirstTrack,
            LastTrack = FirstTrack + 7,
            Last,
        };

        enum class NoteSource : uint8_t {
            FirstTrack,
            LastTrack = FirstTrack + 7,
            FirstNote,
            LastNote = FirstNote + 127,
            Last,
        };

        enum class VelocitySource : uint8_t {
            FirstTrack,
            LastTrack = FirstTrack + 7,
            FirstVelocity,
            LastVelocity = FirstVelocity + 127,
            Last,
        };

        enum class ControlSource : uint8_t {
            FirstTrack,
            LastTrack = FirstTrack + 7,
            Last,
        };

        // target

        const MidiTargetConfig &target() const { return _target; };
              MidiTargetConfig &target()       { return _target; };

        // event

        Event event() const { return _event; }
        void setEvent(Event event) {
            if (event != _event) {
                _event = ModelUtils::clampedEnum(event);
                std::memset(&_data, 0, sizeof(_data));
            }
        }

        void editEvent(int value, bool shift) {
            setEvent(ModelUtils::adjustedEnum(event(), value));
        }

        void printEvent(StringBuilder &str) const {
            str(eventName(event()));
        }

        // gateSource

        GateSource gateSource() const { return _data.note.gateSource; }
        void setGateSource(GateSource gateSource) {
            _data.note.gateSource = ModelUtils::clampedEnum(gateSource);
        }

        void editGateSource(int value, bool shift) {
            setGateSource(ModelUtils::adjustedEnum(gateSource(), value));
        }

        void printGateSource(StringBuilder &str) const {
            printTrackSource(str, gateSource());
        }

        // noteSource

        NoteSource noteSource() const { return _data.note.noteSource; }
        void setNoteSource(NoteSource noteSource) {
            _data.note.noteSource = ModelUtils::clampedEnum(noteSource);
        }

        void editNoteSource(int value, bool shift) {
            setNoteSource(ModelUtils::adjustedEnum(noteSource(), value));
        }

        void printNoteSource(StringBuilder &str) const {
            if (!printTrackSource(str, noteSource())) {
                Types::printMidiNote(str, int(noteSource()) - int(NoteSource::FirstNote));
            }
        }

        // velocitySource

        VelocitySource velocitySource() const { return _data.note.velocitySource; }
        void setVelocitySource(VelocitySource velocitySource) {
            _data.note.velocitySource = ModelUtils::clampedEnum(velocitySource);
        }

        void editVelocitySource(int value, bool shift) {
            setVelocitySource(ModelUtils::adjustedEnum(velocitySource(), value));
        }

        void printVelocitySource(StringBuilder &str) const {
            if (!printTrackSource(str, velocitySource())) {
                str("%d", int(velocitySource()) - int(VelocitySource::FirstVelocity));
            }
        }

        // controlNumber

        int controlNumber() const { return _data.controlChange.controlNumber; }
        void setControlNumber(int controlNumber) {
            _data.controlChange.controlNumber = clamp(controlNumber, 0, 127);
        }

        void editControlNumber(int value, bool shift) {
            setControlNumber(controlNumber() + value);
        }

        void printControlNumber(StringBuilder &str) const {
            str("%d", controlNumber());
        }

        // controlSource

        ControlSource controlSource() const { return _data.controlChange.controlSource; }
        void setControlSource(ControlSource controlSource) {
            _data.controlChange.controlSource = ModelUtils::clampedEnum(controlSource);
        }

        void editControlSource(int value, bool shift) {
            setControlSource(ModelUtils::adjustedEnum(controlSource(), value));
        }

        void printControlSource(StringBuilder &str) const {
            if (!printTrackSource(str, controlSource())) {
            }
        }

        bool isNoteEvent() const {
            return event() == MidiOutput::Output::Event::Note;
        }

        bool isControlChangeEvent() const {
            return event() == MidiOutput::Output::Event::ControlChange;
        }

        bool takesGateFromTrack(int trackIndex) const {
            return isNoteEvent() && int(gateSource()) == trackIndex;
        }

        bool takesNoteFromTrack(int trackIndex) const {
            return isNoteEvent() && int(noteSource()) == trackIndex;
        }

        bool takesVelocityFromTrack(int trackIndex) const {
            return isNoteEvent() && int(velocitySource()) == trackIndex;
        }

        bool takesControlFromTrack(int trackIndex) const {
            return isControlChangeEvent() && int(controlSource()) == trackIndex;
        }


        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

        bool operator==(const Output &other) const;
        bool operator!=(const Output &other) const {
            return !(*this == other);
        }

    private:
        template<typename Enum>
        static bool printTrackSource(StringBuilder &str, Enum value) {
            if (int(value) >= int(Enum::FirstTrack) && int(value) <= int(Enum::LastTrack)) {
                str("Track %d", int(value) - int(Enum::FirstTrack) + 1);
                return true;
            }
            return false;
        }

        MidiTargetConfig _target;
        Event _event;
        union {
            struct Note {
                GateSource gateSource;
                NoteSource noteSource;
                VelocitySource velocitySource;
                bool operator==(const Note &other) const {
                    return gateSource == other.gateSource && noteSource == other.noteSource && velocitySource == other.velocitySource;
                }
            } note;
            struct ControlChange {
                uint8_t controlNumber;
                ControlSource controlSource;
                bool operator==(const ControlChange &other) const {
                    return controlNumber == other.controlNumber && controlSource == other.controlSource;
                }
            } controlChange;
        } _data;
    };

    typedef std::array<Output, CONFIG_MIDI_OUTPUT_COUNT> OutputArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // outputs

    const OutputArray &outputs() const { return _outputs; }
          OutputArray &outputs()       { return _outputs; }

    const Output &output(int index) const { return _outputs[index]; }
          Output &output(int index)       { return _outputs[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    MidiOutput();

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

private:
    OutputArray _outputs;
    bool _dirty;
};
