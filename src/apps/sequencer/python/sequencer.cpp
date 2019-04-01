#include "SequencerApp.h"
#include "model/Model.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

void register_sequencer(py::module &m) {
    // ------------------------------------------------------------------------
    // Sequencer
    // ------------------------------------------------------------------------

    py::class_<SequencerApp> sequencer(m, "Sequencer");
    sequencer
        .def_property_readonly("model", [] (SequencerApp &app) { return &app.model; })
    ;

    // ------------------------------------------------------------------------
    // Model
    // ------------------------------------------------------------------------

    py::class_<Model> model(m, "Model");
    model
        .def_property_readonly("project", [] (Model &model) { return &model.project(); })
    ;

    // ------------------------------------------------------------------------
    // Project
    // ------------------------------------------------------------------------

    py::class_<Project> project(m, "Project");
    project
        .def_property("name", &Project::name, &Project::setName)
        .def_property("slot", &Project::slot, &Project::setSlot)
        .def_property("tempo", &Project::tempo, &Project::setTempo)
        .def_property("swing", &Project::swing, &Project::setSwing)
        .def_property("syncMeasure", &Project::syncMeasure, &Project::setSyncMeasure)
        .def_property("scale", &Project::scale, &Project::setScale)
        .def_property("rootNote", &Project::rootNote, &Project::setRootNote)
        .def_property("recordMode", &Project::recordMode, &Project::setRecordMode)
        .def_property_readonly("clockSetup", [] (Project &project) { return &project.clockSetup(); })
        .def_property_readonly("tracks", [] (Project &project) {
            py::list result;
            for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
                result.append(&project.track(i));
            }
            return result;
        })
        .def("cvOutputTrack", &Project::cvOutputTrack)
        .def("setCvOutputTrack", &Project::setCvOutputTrack)
        .def("gateOutputTrack", &Project::gateOutputTrack)
        .def("setGateOutputTrack", &Project::setGateOutputTrack)
        .def_property_readonly("song", [] (Project &project) { return &project.song(); })
        .def_property_readonly("playState", [] (Project &project) { return &project.playState(); })
        // TODO userScales
        .def_property_readonly("routing", [] (Project &project) { return &project.routing(); })
        .def_property_readonly("midiOutput", [] (Project &project) { return &project.midiOutput(); })
        .def_property("selectedTrackIndex", &Project::selectedTrackIndex, &Project::setSelectedTrackIndex)
        .def_property("selectedPatternIndex", &Project::selectedPatternIndex, &Project::setSelectedPatternIndex)

        .def("clear", &Project::clear)
        .def("clearPattern", &Project::clearPattern)
        .def("setTrackMode", &Project::setTrackMode)
    ;

    // ------------------------------------------------------------------------
    // Types
    // ------------------------------------------------------------------------

    py::class_<Types> types(m, "Types");

    py::enum_<Types::RecordMode>(types, "RecordMode")
        .value("Overdub", Types::RecordMode::Overdub)
        .value("Overwrite", Types::RecordMode::Overwrite)
        .value("StepRecord", Types::RecordMode::StepRecord)
        .export_values()
    ;

    py::enum_<Types::PlayMode>(types, "PlayMode")
        .value("Aligned", Types::PlayMode::Aligned)
        .value("Free", Types::PlayMode::Free)
        .export_values()
    ;

    py::enum_<Types::FillMode>(types, "FillMode")
        .value("None", Types::FillMode::None)
        .value("Gates", Types::FillMode::Gates)
        .value("NextPattern", Types::FillMode::NextPattern)
        .export_values()
    ;

    py::enum_<Types::RunMode>(types, "RunMode")
        .value("Forward", Types::RunMode::Forward)
        .value("Backward", Types::RunMode::Backward)
        .value("Pendulum", Types::RunMode::Pendulum)
        .value("PingPong", Types::RunMode::PingPong)
        .value("Random", Types::RunMode::Random)
        .value("RandomWalk", Types::RunMode::RandomWalk)
        .export_values()
    ;

    py::enum_<Types::VoltageRange>(types, "VoltageRange")
        .value("Unipolar1V", Types::VoltageRange::Unipolar1V)
        .value("Unipolar2V", Types::VoltageRange::Unipolar2V)
        .value("Unipolar3V", Types::VoltageRange::Unipolar3V)
        .value("Unipolar4V", Types::VoltageRange::Unipolar4V)
        .value("Unipolar5V", Types::VoltageRange::Unipolar5V)
        .value("Bipolar1V", Types::VoltageRange::Bipolar1V)
        .value("Bipolar2V", Types::VoltageRange::Bipolar2V)
        .value("Bipolar3V", Types::VoltageRange::Bipolar3V)
        .value("Bipolar4V", Types::VoltageRange::Bipolar4V)
        .value("Bipolar5V", Types::VoltageRange::Bipolar5V)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // ClockSetup
    // ------------------------------------------------------------------------

    py::class_<ClockSetup> clockSetup(m, "ClockSetup");
    clockSetup
        .def_property("mode", &ClockSetup::mode, &ClockSetup::setMode)
        .def_property("shiftMode", &ClockSetup::shiftMode, &ClockSetup::setShiftMode)
        .def_property("clockInputDivisor", &ClockSetup::clockInputDivisor, &ClockSetup::setClockInputDivisor)
        .def_property("clockInputMode", &ClockSetup::clockInputMode, &ClockSetup::setClockInputMode)
        .def_property("clockOutputDivisor", &ClockSetup::clockOutputDivisor, &ClockSetup::setClockOutputDivisor)
        .def_property("clockOutputPulse", &ClockSetup::clockOutputPulse, &ClockSetup::setClockOutputPulse)
        .def_property("clockOutputMode", &ClockSetup::clockOutputMode, &ClockSetup::setClockOutputMode)
        .def_property("midiRx", &ClockSetup::midiRx, &ClockSetup::setMidiRx)
        .def_property("midiTx", &ClockSetup::midiTx, &ClockSetup::setMidiTx)
        .def_property("usbRx", &ClockSetup::usbRx, &ClockSetup::setUsbRx)
        .def_property("usbTx", &ClockSetup::usbTx, &ClockSetup::setUsbTx)
        .def("clear", &ClockSetup::clear)
    ;

    py::enum_<ClockSetup::Mode>(clockSetup, "Mode")
        .value("Auto", ClockSetup::Mode::Auto)
        .value("Master", ClockSetup::Mode::Master)
        .value("Slave", ClockSetup::Mode::Slave)
        .export_values()
    ;

    py::enum_<ClockSetup::ShiftMode>(clockSetup, "ShiftMode")
        .value("Restart", ClockSetup::ShiftMode::Restart)
        .value("Pause", ClockSetup::ShiftMode::Pause)
        .export_values()
    ;

    py::enum_<ClockSetup::ClockInputMode>(clockSetup, "ClockInputMode")
        .value("Reset", ClockSetup::ClockInputMode::Reset)
        .value("Run", ClockSetup::ClockInputMode::Run)
        .value("StartStop", ClockSetup::ClockInputMode::StartStop)
        .export_values()
    ;

    py::enum_<ClockSetup::ClockOutputMode>(clockSetup, "ClockOutputMode")
        .value("Reset", ClockSetup::ClockOutputMode::Reset)
        .value("Run", ClockSetup::ClockOutputMode::Run)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // Track
    // ------------------------------------------------------------------------

    py::class_<Track> track(m, "Track");
    track
        .def_property_readonly("trackIndex", &Track::trackIndex)
        .def_property_readonly("trackMode", &Track::trackMode)
        .def_property("linkTrack", &Track::linkTrack, &Track::setLinkTrack)
        .def_property_readonly("noteTrack", [] (Track &track) { return &track.noteTrack(); })
        .def_property_readonly("curveTrack", [] (Track &track) { return &track.curveTrack(); })
        .def_property_readonly("midiCvTrack", [] (Track &track) { return &track.midiCvTrack(); })
        .def("clear", &Track::clear)
        .def("clearPattern", &Track::clearPattern)
    ;

    py::enum_<Track::TrackMode>(track, "TrackMode")
        .value("Note", Track::TrackMode::Note)
        .value("Curve", Track::TrackMode::Curve)
        .value("MidiCv", Track::TrackMode::MidiCv)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // NoteTrack
    // ------------------------------------------------------------------------

    py::class_<NoteTrack> noteTrack(m, "NoteTrack");
    noteTrack
        .def_property("playMode", &NoteTrack::playMode, &NoteTrack::setPlayMode)
        .def_property("fillMode", &NoteTrack::fillMode, &NoteTrack::setFillMode)
        .def_property("slideTime", &NoteTrack::slideTime, &NoteTrack::setSlideTime)
        .def_property("octave", &NoteTrack::octave, &NoteTrack::setOctave)
        .def_property("transpose", &NoteTrack::transpose, &NoteTrack::setTranspose)
        .def_property("rotate", &NoteTrack::rotate, &NoteTrack::setRotate)
        .def_property("gateProbabilityBias", &NoteTrack::gateProbabilityBias, &NoteTrack::setGateProbabilityBias)
        .def_property("retriggerProbabilityBias", &NoteTrack::retriggerProbabilityBias, &NoteTrack::setRetriggerProbabilityBias)
        .def_property("lengthBias", &NoteTrack::lengthBias, &NoteTrack::setLengthBias)
        .def_property("noteProbabilityBias", &NoteTrack::noteProbabilityBias, &NoteTrack::setNoteProbabilityBias)
        .def_property_readonly("sequences", [] (NoteTrack &noteTrack) {
            py::list result;
            for (int i = 0; i < CONFIG_PATTERN_COUNT; ++i) {
                result.append(&noteTrack.sequence(i));
            }
            return result;
        })
        .def("clear", &NoteTrack::clear)
    ;

    // ------------------------------------------------------------------------
    // CurveTrack
    // ------------------------------------------------------------------------

    py::class_<CurveTrack> curveTrack(m, "CurveTrack");
    curveTrack
        .def_property("playMode", &CurveTrack::playMode, &CurveTrack::setPlayMode)
        .def_property("fillMode", &CurveTrack::fillMode, &CurveTrack::setFillMode)
        .def_property("rotate", &CurveTrack::rotate, &CurveTrack::setRotate)
        .def_property_readonly("sequences", [] (CurveTrack &curveTrack) {
            py::list result;
            for (int i = 0; i < CONFIG_PATTERN_COUNT; ++i) {
                result.append(&curveTrack.sequence(i));
            }
            return result;
        })
        .def("clear", &CurveTrack::clear)
    ;

    // ------------------------------------------------------------------------
    // MidiCvTrack
    // ------------------------------------------------------------------------

    py::class_<MidiCvTrack> midiCvTrack(m, "MidiCvTrack");
    midiCvTrack
        .def_property_readonly("source", [] (MidiCvTrack &midiCvTrack) { return &midiCvTrack.source(); })
        .def_property("voices", &MidiCvTrack::voices, &MidiCvTrack::setVoices)
        .def_property("voiceConfig", &MidiCvTrack::voiceConfig, &MidiCvTrack::setVoiceConfig)
        .def_property("pitchBendRange", &MidiCvTrack::pitchBendRange, &MidiCvTrack::setPitchBendRange)
        .def_property("modulationRange", &MidiCvTrack::modulationRange, &MidiCvTrack::setModulationRange)
        .def_property("retrigger", &MidiCvTrack::retrigger, &MidiCvTrack::setRetrigger)
        .def("clear", &MidiCvTrack::clear)
    ;

    py::enum_<MidiCvTrack::VoiceConfig>(midiCvTrack, "VoiceConfig")
        .value("Pitch", MidiCvTrack::VoiceConfig::Pitch)
        .value("PitchVelocity", MidiCvTrack::VoiceConfig::PitchVelocity)
        .value("PitchVelocityPressure", MidiCvTrack::VoiceConfig::PitchVelocityPressure)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // NoteSequence
    // ------------------------------------------------------------------------

    py::class_<NoteSequence> noteSequence(m, "NoteSequence");
    noteSequence
        .def_property("scale", &NoteSequence::scale, &NoteSequence::setScale)
        .def_property("rootNote", &NoteSequence::rootNote, &NoteSequence::setRootNote)
        .def_property("divisor", &NoteSequence::divisor, &NoteSequence::setDivisor)
        .def_property("resetMeasure", &NoteSequence::resetMeasure, &NoteSequence::setResetMeasure)
        .def_property("runMode", &NoteSequence::runMode, &NoteSequence::setRunMode)
        .def_property("firstStep", &NoteSequence::firstStep, &NoteSequence::setFirstStep)
        .def_property("lastStep", &NoteSequence::lastStep, &NoteSequence::setLastStep)
        .def_property_readonly("steps", [] (NoteSequence &noteSequence) {
            py::list result;
            for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
                result.append(&noteSequence.step(i));
            }
            return result;
        })
        .def("clear", &NoteSequence::clear)
        .def("clearSteps", &NoteSequence::clearSteps)
    ;

    py::class_<NoteSequence::Step> noteSequenceStep(noteSequence, "Step");
    noteSequenceStep
        .def_property("gate", &NoteSequence::Step::gate, &NoteSequence::Step::setGate)
        .def_property("gateProbability", &NoteSequence::Step::gateProbability, &NoteSequence::Step::setGateProbability)
        .def_property("slide", &NoteSequence::Step::slide, &NoteSequence::Step::setSlide)
        .def_property("retrigger", &NoteSequence::Step::retrigger, &NoteSequence::Step::setRetrigger)
        .def_property("retriggerProbability", &NoteSequence::Step::retriggerProbability, &NoteSequence::Step::setRetriggerProbability)
        .def_property("length", &NoteSequence::Step::length, &NoteSequence::Step::setLength)
        .def_property("lengthVariationRange", &NoteSequence::Step::lengthVariationRange, &NoteSequence::Step::setLengthVariationRange)
        .def_property("lengthVariationProbability", &NoteSequence::Step::lengthVariationProbability, &NoteSequence::Step::setLengthVariationProbability)
        .def_property("note", &NoteSequence::Step::note, &NoteSequence::Step::setNote)
        .def_property("noteVariationRange", &NoteSequence::Step::noteVariationRange, &NoteSequence::Step::setNoteVariationRange)
        .def_property("noteVariationProbability", &NoteSequence::Step::noteVariationProbability, &NoteSequence::Step::setNoteVariationProbability)
        .def("clear", &NoteSequence::Step::clear)
    ;

    // ------------------------------------------------------------------------
    // CurveSequence
    // ------------------------------------------------------------------------

    py::class_<CurveSequence> curveSequence(m, "CurveSequence");
    curveSequence
        .def_property("range", &CurveSequence::range, &CurveSequence::setRange)
        .def_property("divisor", &CurveSequence::divisor, &CurveSequence::setDivisor)
        .def_property("resetMeasure", &CurveSequence::resetMeasure, &CurveSequence::setResetMeasure)
        .def_property("runMode", &CurveSequence::runMode, &CurveSequence::setRunMode)
        .def_property("firstStep", &CurveSequence::firstStep, &CurveSequence::setFirstStep)
        .def_property("lastStep", &CurveSequence::lastStep, &CurveSequence::setLastStep)
        .def_property_readonly("steps", [] (CurveSequence &curveSequence) {
            py::list result;
            for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
                result.append(&curveSequence.step(i));
            }
            return result;
        })
        .def("clear", &CurveSequence::clear)
        .def("clearSteps", &CurveSequence::clearSteps)
    ;

    py::class_<CurveSequence::Step> curveSequenceStep(curveSequence, "Step");
    curveSequenceStep
        .def_property("shape", &CurveSequence::Step::shape, &CurveSequence::Step::setShape)
        .def_property("min", &CurveSequence::Step::min, &CurveSequence::Step::setMin)
        .def_property("max", &CurveSequence::Step::max, &CurveSequence::Step::setMax)
        .def("clear", &CurveSequence::Step::clear)
    ;

    // ------------------------------------------------------------------------
    // Song
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // PlayState
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Routing
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // MidiOutput
    // ------------------------------------------------------------------------

}
