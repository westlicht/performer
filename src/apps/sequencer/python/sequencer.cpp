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
        .def_property("timeSignature", &Project::timeSignature, &Project::setTimeSignature)
        .def_property("syncMeasure", &Project::syncMeasure, &Project::setSyncMeasure)
        .def_property("scale", &Project::scale, &Project::setScale)
        .def_property("rootNote", &Project::rootNote, &Project::setRootNote)
        .def_property("recordMode", &Project::recordMode, &Project::setRecordMode)
        .def_property("cvGateInput", &Project::cvGateInput, &Project::setCvGateInput)
        .def_property("curveCvInput", &Project::curveCvInput, &Project::setCurveCvInput)
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
        .def_property("selectedNoteSequenceLayer", &Project::selectedNoteSequenceLayer, &Project::setSelectedNoteSequenceLayer)
        .def_property("selectedCurveSequenceLayer", &Project::selectedCurveSequenceLayer, &Project::setSelectedCurveSequenceLayer)
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

    py::enum_<Types::CvGateInput>(types, "CvGateInput")
        .value("Off", Types::CvGateInput::Off)
        .value("Cv1Cv2", Types::CvGateInput::Cv1Cv2)
        .value("Cv3Cv4", Types::CvGateInput::Cv3Cv4)
        .export_values()
    ;

    py::enum_<Types::CurveCvInput>(types, "CurveCvInput")
        .value("Off", Types::CurveCvInput::Off)
        .value("Cv1", Types::CurveCvInput::Cv1)
        .value("Cv2", Types::CurveCvInput::Cv2)
        .value("Cv3", Types::CurveCvInput::Cv3)
        .value("Cv4", Types::CurveCvInput::Cv4)
        .export_values()
    ;

    py::enum_<Types::PlayMode>(types, "PlayMode")
        .value("Aligned", Types::PlayMode::Aligned)
        .value("Free", Types::PlayMode::Free)
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

    py::enum_<Types::MidiPort>(types, "MidiPort")
        .value("Midi", Types::MidiPort::Midi)
        .value("UsbMidi", Types::MidiPort::UsbMidi)
        .export_values()
    ;

    py::enum_<Types::Condition>(types, "Condition")
        .value("Off", Types::Condition::Off)
        .value("Fill", Types::Condition::Fill)
        .value("NotFill", Types::Condition::NotFill)
        .value("Pre", Types::Condition::Pre)
        .value("NotPre", Types::Condition::NotPre)
        .value("First", Types::Condition::First)
        .value("NotFirst", Types::Condition::NotFirst)
        .value("Loop2", Types::Condition::Loop2)
        .value("Loop3", Types::Condition::Loop3)
        .value("Loop4", Types::Condition::Loop4)
        .value("Loop5", Types::Condition::Loop5)
        .value("Loop6", Types::Condition::Loop6)
        .value("Loop7", Types::Condition::Loop7)
        .value("Loop8", Types::Condition::Loop8)
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
        .def_property("clockOutputSwing", &ClockSetup::clockOutputSwing, &ClockSetup::setClockOutputSwing)
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
        .def("copyPattern", &Track::copyPattern)
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
        .def_property("cvUpdateMode", &NoteTrack::cvUpdateMode, &NoteTrack::setCvUpdateMode)
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

    py::enum_<NoteTrack::FillMode>(noteTrack, "FillMode")
        .value("None", NoteTrack::FillMode::None)
        .value("Gates", NoteTrack::FillMode::Gates)
        .value("NextPattern", NoteTrack::FillMode::NextPattern)
        .value("Condition", NoteTrack::FillMode::Condition)
        .export_values()
    ;

    py::enum_<NoteTrack::CvUpdateMode>(noteTrack, "CvUpdateMode")
        .value("Gate", NoteTrack::CvUpdateMode::Gate)
        .value("Always", NoteTrack::CvUpdateMode::Always)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // CurveTrack
    // ------------------------------------------------------------------------

    py::class_<CurveTrack> curveTrack(m, "CurveTrack");
    curveTrack
        .def_property("playMode", &CurveTrack::playMode, &CurveTrack::setPlayMode)
        .def_property("fillMode", &CurveTrack::fillMode, &CurveTrack::setFillMode)
        .def_property("slideTime", &CurveTrack::slideTime, &CurveTrack::setSlideTime)
        .def_property("rotate", &CurveTrack::rotate, &CurveTrack::setRotate)
        .def_property("shapeProbabilityBias", &CurveTrack::shapeProbabilityBias, &CurveTrack::setShapeProbabilityBias)
        .def_property("gateProbabilityBias", &CurveTrack::gateProbabilityBias, &CurveTrack::setGateProbabilityBias)
        .def_property_readonly("sequences", [] (CurveTrack &curveTrack) {
            py::list result;
            for (int i = 0; i < CONFIG_PATTERN_COUNT; ++i) {
                result.append(&curveTrack.sequence(i));
            }
            return result;
        })
        .def("clear", &CurveTrack::clear)
    ;

    py::enum_<CurveTrack::FillMode>(curveTrack, "FillMode")
        .value("None", CurveTrack::FillMode::None)
        .value("Variation", CurveTrack::FillMode::Variation)
        .value("NextPattern", CurveTrack::FillMode::NextPattern)
        .value("Invert", CurveTrack::FillMode::Invert)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // MidiCvTrack
    // ------------------------------------------------------------------------

    py::class_<MidiCvTrack> midiCvTrack(m, "MidiCvTrack");
    midiCvTrack
        .def_property_readonly("source", [] (MidiCvTrack &midiCvTrack) { return &midiCvTrack.source(); })
        .def_property("voices", &MidiCvTrack::voices, &MidiCvTrack::setVoices)
        .def_property("voiceConfig", &MidiCvTrack::voiceConfig, &MidiCvTrack::setVoiceConfig)
        .def_property("notePriority", &MidiCvTrack::notePriority, &MidiCvTrack::setNotePriority)
        .def_property("lowNote", &MidiCvTrack::lowNote, &MidiCvTrack::setLowNote)
        .def_property("highNote", &MidiCvTrack::highNote, &MidiCvTrack::setHighNote)
        .def_property("pitchBendRange", &MidiCvTrack::pitchBendRange, &MidiCvTrack::setPitchBendRange)
        .def_property("modulationRange", &MidiCvTrack::modulationRange, &MidiCvTrack::setModulationRange)
        .def_property("retrigger", &MidiCvTrack::retrigger, &MidiCvTrack::setRetrigger)
        .def_property("slideTime", &MidiCvTrack::slideTime, &MidiCvTrack::setSlideTime)
        .def_property("transpose", &MidiCvTrack::transpose, &MidiCvTrack::setTranspose)
        .def_property_readonly("arpeggiator", [] (MidiCvTrack &midiCvTrack) { return &midiCvTrack.arpeggiator(); })
        .def("clear", &MidiCvTrack::clear)
    ;

    py::enum_<MidiCvTrack::VoiceConfig>(midiCvTrack, "VoiceConfig")
        .value("Pitch", MidiCvTrack::VoiceConfig::Pitch)
        .value("PitchVelocity", MidiCvTrack::VoiceConfig::PitchVelocity)
        .value("PitchVelocityPressure", MidiCvTrack::VoiceConfig::PitchVelocityPressure)
        .export_values()
    ;

    py::enum_<MidiCvTrack::NotePriority>(midiCvTrack, "NotePriority")
        .value("LastNote", MidiCvTrack::NotePriority::LastNote)
        .value("FirstNote", MidiCvTrack::NotePriority::FirstNote)
        .value("LowestNote", MidiCvTrack::NotePriority::LowestNote)
        .value("HighestNote", MidiCvTrack::NotePriority::HighestNote)
        .export_values()
    ;

    // ------------------------------------------------------------------------
    // Arpeggiator
    // ------------------------------------------------------------------------

    py::class_<Arpeggiator> arpeggiator(m, "Arpeggiator");
    arpeggiator
        .def_property("enabled", &Arpeggiator::enabled, &Arpeggiator::setEnabled)
        .def_property("hold", &Arpeggiator::hold, &Arpeggiator::setHold)
        .def_property("mode", &Arpeggiator::mode, &Arpeggiator::setMode)
        .def_property("divisor", &Arpeggiator::divisor, &Arpeggiator::setDivisor)
        .def_property("gateLength", &Arpeggiator::gateLength, &Arpeggiator::setGateLength)
        .def_property("octaves", &Arpeggiator::octaves, &Arpeggiator::setOctaves)
        .def("clear", &Arpeggiator::clear)
    ;

    py::enum_<Arpeggiator::Mode>(arpeggiator, "Mode")
        .value("PlayOrder", Arpeggiator::Mode::PlayOrder)
        .value("Up", Arpeggiator::Mode::Up)
        .value("Down", Arpeggiator::Mode::Down)
        .value("UpDown", Arpeggiator::Mode::UpDown)
        .value("DownUp", Arpeggiator::Mode::DownUp)
        .value("UpAndDown", Arpeggiator::Mode::UpAndDown)
        .value("DownAndUp", Arpeggiator::Mode::DownAndUp)
        .value("Converge", Arpeggiator::Mode::Converge)
        .value("Diverge", Arpeggiator::Mode::Diverge)
        .value("Random", Arpeggiator::Mode::Random)
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
        .def("shiftSteps", &NoteSequence::shiftSteps)
        .def("duplicateSteps", &NoteSequence::duplicateSteps)
    ;

    py::enum_<NoteSequence::Layer>(noteSequence, "Layer")
        .value("Gate", NoteSequence::Layer::Gate)
        .value("GateProbability", NoteSequence::Layer::GateProbability)
        .value("GateOffset", NoteSequence::Layer::GateOffset)
        .value("Slide", NoteSequence::Layer::Slide)
        .value("Retrigger", NoteSequence::Layer::Retrigger)
        .value("RetriggerProbability", NoteSequence::Layer::RetriggerProbability)
        .value("Length", NoteSequence::Layer::Length)
        .value("LengthVariationRange", NoteSequence::Layer::LengthVariationRange)
        .value("LengthVariationProbability", NoteSequence::Layer::LengthVariationProbability)
        .value("Note", NoteSequence::Layer::Note)
        .value("NoteVariationRange", NoteSequence::Layer::NoteVariationRange)
        .value("NoteVariationProbability", NoteSequence::Layer::NoteVariationProbability)
        .value("Condition", NoteSequence::Layer::Condition)
        .export_values()
    ;

    py::class_<NoteSequence::Step> noteSequenceStep(noteSequence, "Step");
    noteSequenceStep
        .def_property("gate", &NoteSequence::Step::gate, &NoteSequence::Step::setGate)
        .def_property("gateProbability", &NoteSequence::Step::gateProbability, &NoteSequence::Step::setGateProbability)
        .def_property("gateOffset", &NoteSequence::Step::gateOffset, &NoteSequence::Step::setGateOffset)
        .def_property("slide", &NoteSequence::Step::slide, &NoteSequence::Step::setSlide)
        .def_property("retrigger", &NoteSequence::Step::retrigger, &NoteSequence::Step::setRetrigger)
        .def_property("retriggerProbability", &NoteSequence::Step::retriggerProbability, &NoteSequence::Step::setRetriggerProbability)
        .def_property("length", &NoteSequence::Step::length, &NoteSequence::Step::setLength)
        .def_property("lengthVariationRange", &NoteSequence::Step::lengthVariationRange, &NoteSequence::Step::setLengthVariationRange)
        .def_property("lengthVariationProbability", &NoteSequence::Step::lengthVariationProbability, &NoteSequence::Step::setLengthVariationProbability)
        .def_property("note", &NoteSequence::Step::note, &NoteSequence::Step::setNote)
        .def_property("noteVariationRange", &NoteSequence::Step::noteVariationRange, &NoteSequence::Step::setNoteVariationRange)
        .def_property("noteVariationProbability", &NoteSequence::Step::noteVariationProbability, &NoteSequence::Step::setNoteVariationProbability)
        .def_property("condition", &NoteSequence::Step::condition, &NoteSequence::Step::setCondition)
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
        .def("shiftSteps", &CurveSequence::shiftSteps)
        .def("duplicateSteps", &CurveSequence::duplicateSteps)
    ;

    py::enum_<CurveSequence::Layer>(curveSequence, "Layer")
        .value("Shape", CurveSequence::Layer::Shape)
        .value("ShapeVariation", CurveSequence::Layer::ShapeVariation)
        .value("ShapeVariationProbability", CurveSequence::Layer::ShapeVariationProbability)
        .value("Min", CurveSequence::Layer::Min)
        .value("Max", CurveSequence::Layer::Max)
        .value("Gate", CurveSequence::Layer::Gate)
        .value("GateProbability", CurveSequence::Layer::GateProbability)
        .export_values()
    ;

    py::class_<CurveSequence::Step> curveSequenceStep(curveSequence, "Step");
    curveSequenceStep
        .def_property("shape", &CurveSequence::Step::shape, &CurveSequence::Step::setShape)
        .def_property("shapeVariation", &CurveSequence::Step::shapeVariation, &CurveSequence::Step::setShapeVariation)
        .def_property("shapeVariationProbability", &CurveSequence::Step::shapeVariationProbability, &CurveSequence::Step::setShapeVariationProbability)
        .def_property("min", &CurveSequence::Step::min, &CurveSequence::Step::setMin)
        .def_property("max", &CurveSequence::Step::max, &CurveSequence::Step::setMax)
        .def_property("gate", &CurveSequence::Step::gate, &CurveSequence::Step::setGate)
        .def_property("gateProbability", &CurveSequence::Step::gateProbability, &CurveSequence::Step::setGateProbability)
        .def("clear", &CurveSequence::Step::clear)
    ;

    // ------------------------------------------------------------------------
    // Song
    // ------------------------------------------------------------------------

    py::class_<Song> song(m, "Song");
    song
        .def_property_readonly("slots", [] (Song &song) {
            py::list result;
            for (int i = 0; i < CONFIG_SONG_SLOT_COUNT; ++i) {
                result.append(&song.slot(i));
            }
            return result;
        })
        .def("chainPattern", &Song::chainPattern)
        .def("insertSlot", &Song::insertSlot)
        .def("removeSlot", &Song::removeSlot)
        .def("swapSlot", &Song::swapSlot)
        // TODO .def("setPattern", &Song::setPattern)
        .def("setRepeats", &Song::setRepeats)
        .def("clear", &Song::clear)
    ;

    py::class_<Song::Slot> slot(song, "Slot");
    slot
        .def("pattern", &Song::Slot::pattern)
        .def_property_readonly("repeats", &Song::Slot::repeats)
        .def("clear", &Song::Slot::clear)
    ;

    // ------------------------------------------------------------------------
    // PlayState
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Routing
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // MidiOutput
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // TimeSignature
    // ------------------------------------------------------------------------

    py::class_<TimeSignature> timeSignature(m, "TimeSignature");
    timeSignature
        .def_property("beats", &TimeSignature::beats, &TimeSignature::setBeats)
        .def_property("note", &TimeSignature::note, &TimeSignature::setNote)
    ;

    // ------------------------------------------------------------------------
    // MidiConfig
    // ------------------------------------------------------------------------

    py::class_<MidiSourceConfig> midiSourceConfig(m, "MidiSourceConfig");
    midiSourceConfig
        .def_property("port", &MidiSourceConfig::port, &MidiSourceConfig::setPort)
        .def_property("channel", &MidiSourceConfig::channel, &MidiSourceConfig::setChannel)
        .def("clear", &MidiSourceConfig::clear)
    ;

    py::class_<MidiTargetConfig> midiTargetConfig(m, "MidiTargetConfig");
    midiTargetConfig
        .def_property("port", &MidiTargetConfig::port, &MidiTargetConfig::setPort)
        .def_property("channel", &MidiTargetConfig::channel, &MidiTargetConfig::setChannel)
        .def("clear", &MidiTargetConfig::clear)
    ;
}
