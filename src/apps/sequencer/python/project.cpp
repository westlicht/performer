#include "model/Project.h"
#include "model/ProjectVersion.h"

#include <pybind11/pybind11.h>

#include <string>
#include <fstream>
#include <exception>

namespace py = pybind11;
using namespace py::literals;

static void loadProject(Project &project, const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.good()) {
        throw std::runtime_error("Cannot open file");
    }

    FileHeader header;
    ifs.read(reinterpret_cast<char *>(&header), sizeof(header));

    VersionedSerializedReader reader(
        [&ifs] (void *data, size_t len) { ifs.read(reinterpret_cast<char *>(data), len); },
        ProjectVersion::Latest
    );

    if (!project.read(reader)) {
        throw std::runtime_error("Failed to load project");
    }
}

static void saveProject(const Project &project, const std::string &filename) {
    std::ofstream ofs(filename);
    if (!ofs.good()) {
        throw std::runtime_error("Cannot open file");
    }

    FileHeader header(FileType::Project, 0, project.name());
    ofs.write(reinterpret_cast<const char *>(&header), sizeof(header));

    VersionedSerializedWriter writer(
        [&ofs] (const void *data, size_t len) { ofs.write(reinterpret_cast<const char *>(data), len); },
        ProjectVersion::Latest
    );

    project.write(writer);
}

void register_project(py::module &m) {
    // ------------------------------------------------------------------------
    // Project
    // ------------------------------------------------------------------------

    py::class_<Project> project(m, "Project");
    project
        .def(py::init<>())
        .def_property("name", &Project::name, &Project::setName)
        .def_property("slot", &Project::slot, &Project::setSlot)
        .def_property("tempo", &Project::tempo, &Project::setTempo)
        .def_property("swing", &Project::swing, &Project::setSwing)
        .def_property("timeSignature", &Project::timeSignature, &Project::setTimeSignature)
        .def_property("syncMeasure", &Project::syncMeasure, &Project::setSyncMeasure)
        .def_property("scale", &Project::scale, &Project::setScale)
        .def_property("rootNote", &Project::rootNote, &Project::setRootNote)
        .def_property("monitorMode", &Project::monitorMode, &Project::setMonitorMode)
        .def_property("recordMode", &Project::recordMode, &Project::setRecordMode)
        .def_property("midiInputMode", &Project::midiInputMode, &Project::setMidiInputMode)
        .def_property_readonly("midiInputSource", [] (Project &project) { return &project.midiInputSource(); })
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
        .def("cvOutputTrack", &Project::cvOutputTrack, "index"_a)
        .def("setCvOutputTrack", &Project::setCvOutputTrack, "index"_a, "trackIndex"_a)
        .def("gateOutputTrack", &Project::gateOutputTrack, "index"_a)
        .def("setGateOutputTrack", &Project::setGateOutputTrack, "index"_a, "trackIndex"_a)
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
        .def("clearPattern", &Project::clearPattern, "patternIndex"_a)
        .def("setTrackMode", &Project::setTrackMode, "trackIndex"_a, "trackMode"_a)
        .def("load", &loadProject, "filename"_a)
        .def("save", &saveProject, "filename"_a)
    ;

    // ------------------------------------------------------------------------
    // Types
    // ------------------------------------------------------------------------

    py::class_<Types> types(m, "Types");

    py::enum_<Types::MonitorMode>(types, "MonitorMode")
        .value("Always", Types::MonitorMode::Always)
        .value("Stopped", Types::MonitorMode::Stopped)
        .value("Off", Types::MonitorMode::Off)
        .export_values()
    ;

    py::enum_<Types::RecordMode>(types, "RecordMode")
        .value("Overdub", Types::RecordMode::Overdub)
        .value("Overwrite", Types::RecordMode::Overwrite)
        .value("StepRecord", Types::RecordMode::StepRecord)
        .export_values()
    ;

    py::enum_<Types::MidiInputMode>(types, "MidiInputMode")
        .value("Off", Types::MidiInputMode::Off)
        .value("All", Types::MidiInputMode::All)
        .value("Source", Types::MidiInputMode::Source)
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

    py::enum_<Types::Condition> condition(types, "Condition");
    condition
        .value("Off", Types::Condition::Off)
        .value("Fill", Types::Condition::Fill)
        .value("NotFill", Types::Condition::NotFill)
        .value("Pre", Types::Condition::Pre)
        .value("NotPre", Types::Condition::NotPre)
        .value("First", Types::Condition::First)
        .value("NotFirst", Types::Condition::NotFirst)
    ;
    {
        int index = 0;
        for (int loop = 2; loop <= 8; ++loop) {
            for (int step = 1; step <= loop; ++step) {
                condition.value(("Loop" + std::to_string(step) + std::to_string(loop)).c_str(), Types::Condition(int(Types::Condition::Loop2) + index));
                condition.value(("NotLoop" + std::to_string(step) + std::to_string(loop)).c_str(), Types::Condition(int(Types::Condition::NotLoop2) + index));
                ++index;
            }
        }
    }
    condition.export_values();

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
        .def("clearPattern", &Track::clearPattern, "patternIndex"_a)
        .def("copyPattern", &Track::copyPattern, "srcIndex"_a, "dstIndex"_a)
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
        .def_property("muteMode", &CurveTrack::muteMode, &CurveTrack::setMuteMode)
        .def_property("slideTime", &CurveTrack::slideTime, &CurveTrack::setSlideTime)
        .def_property("offset", &CurveTrack::offset, &CurveTrack::setOffset)
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

    py::enum_<CurveTrack::MuteMode>(curveTrack, "MuteMode")
        .value("LastValue", CurveTrack::MuteMode::LastValue)
        .value("Zero", CurveTrack::MuteMode::Zero)
        .value("Min", CurveTrack::MuteMode::Min)
        .value("Max", CurveTrack::MuteMode::Max)
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
        .def("shiftSteps", &NoteSequence::shiftSteps, "selected"_a, "direction"_a)
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
        .def("shiftSteps", &CurveSequence::shiftSteps, "selected"_a, "direction"_a)
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
        .def("chainPattern", &Song::chainPattern, "patternIndex"_a)
        .def("insertSlot", &Song::insertSlot, "slotIndex"_a)
        .def("removeSlot", &Song::removeSlot, "slotIndex"_a)
        .def("duplicateSlot", &Song::duplicateSlot, "slotIndex"_a)
        .def("swapSlot", &Song::swapSlot, "fromIndex"_a, "toIndex"_a)
        .def("setPattern", (void (Song::*)(int, int)) &Song::setPattern, "slotIndex"_a, "pattern"_a)
        .def("setPattern", (void (Song::*)(int, int, int)) &Song::setPattern, "slotIndex"_a, "trackIndex"_a, "pattern"_a)
        .def("setMute", &Song::setMute, "slotIndex"_a, "trackIndex"_a, "mute"_a)
        .def("setRepeats", &Song::setRepeats, "slotIndex"_a, "repeats"_a)
        .def("clear", &Song::clear)
    ;

    py::class_<Song::Slot> slot(song, "Slot");
    slot
        .def("pattern", &Song::Slot::pattern, "trackIndex"_a)
        .def("mute", &Song::Slot::mute, "trackIndex"_a)
        .def_property_readonly("repeats", &Song::Slot::repeats)
        .def("clear", &Song::Slot::clear)
    ;

    // ------------------------------------------------------------------------
    // PlayState
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // Routing
    // ------------------------------------------------------------------------

    py::class_<Routing> routing(m, "Routing");
    routing
        .def_property_readonly("routes", [] (Routing &routing) {
            py::list result;
            for (int i = 0; i < CONFIG_ROUTE_COUNT; ++i) {
                result.append(&routing.route(i));
            }
            return result;
        })
        .def("clear", &Routing::clear)
    ;

    py::enum_<Routing::Target> target(routing, "Target");
    target
        .value("None", Routing::Target::None)
        .value("Play", Routing::Target::Play)
        .value("Record", Routing::Target::Record)
        .value("TapTempo", Routing::Target::TapTempo)
        .value("Tempo", Routing::Target::Tempo)
        .value("Swing", Routing::Target::Swing)
        .value("Mute", Routing::Target::Mute)
        .value("Fill", Routing::Target::Fill)
        .value("FillAmount", Routing::Target::FillAmount)
        .value("Pattern", Routing::Target::Pattern)
        .value("SlideTime", Routing::Target::SlideTime)
        .value("Octave", Routing::Target::Octave)
        .value("Transpose", Routing::Target::Transpose)
        .value("Offset", Routing::Target::Offset)
        .value("Rotate", Routing::Target::Rotate)
        .value("GateProbabilityBias", Routing::Target::GateProbabilityBias)
        .value("RetriggerProbabilityBias", Routing::Target::RetriggerProbabilityBias)
        .value("LengthBias", Routing::Target::LengthBias)
        .value("NoteProbabilityBias", Routing::Target::NoteProbabilityBias)
        .value("ShapeProbabilityBias", Routing::Target::ShapeProbabilityBias)
        .value("FirstStep", Routing::Target::FirstStep)
        .value("LastStep", Routing::Target::LastStep)
        .value("RunMode", Routing::Target::RunMode)
        .value("Divisor", Routing::Target::Divisor)
        .value("Scale", Routing::Target::Scale)
        .value("RootNote", Routing::Target::RootNote)
        .export_values()
    ;

    py::enum_<Routing::Source> source(routing, "Source");
    source
        .value("None", Routing::Source::None)
        .value("CvIn1", Routing::Source::CvIn1)
        .value("CvIn2", Routing::Source::CvIn2)
        .value("CvIn3", Routing::Source::CvIn3)
        .value("CvIn4", Routing::Source::CvIn4)
        .value("CvOut1", Routing::Source::CvOut1)
        .value("CvOut2", Routing::Source::CvOut2)
        .value("CvOut3", Routing::Source::CvOut3)
        .value("CvOut4", Routing::Source::CvOut4)
        .value("CvOut5", Routing::Source::CvOut5)
        .value("CvOut6", Routing::Source::CvOut6)
        .value("CvOut7", Routing::Source::CvOut7)
        .value("CvOut8", Routing::Source::CvOut8)
        .value("Midi", Routing::Source::Midi)
        .export_values()
    ;

    py::class_<Routing::CvSource> cvSource(routing, "CvSource");
    cvSource
        .def_property("range", &Routing::CvSource::range, &Routing::CvSource::setRange)
        .def("clear", &Routing::CvSource::clear)
    ;

    py::class_<Routing::MidiSource> midiSource(routing, "MidiSource");
    midiSource
        .def_property_readonly("source", [] (Routing::MidiSource &midiSource) { return &midiSource.source(); })
        .def_property("event", &Routing::MidiSource::event, &Routing::MidiSource::setEvent)
        .def_property("controlNumber", &Routing::MidiSource::controlNumber, &Routing::MidiSource::setControlNumber)
        .def_property("note", &Routing::MidiSource::note, &Routing::MidiSource::setNote)
        .def_property("noteRange", &Routing::MidiSource::noteRange, &Routing::MidiSource::setNoteRange)
        .def("clear", &Routing::MidiSource::clear)
    ;

    py::enum_<Routing::MidiSource::Event> event(midiSource, "Event");
    event
        .value("ControlAbsolute", Routing::MidiSource::Event::ControlAbsolute)
        .value("ControlRelative", Routing::MidiSource::Event::ControlRelative)
        .value("PitchBend", Routing::MidiSource::Event::PitchBend)
        .value("NoteMomentary", Routing::MidiSource::Event::NoteMomentary)
        .value("NoteToggle", Routing::MidiSource::Event::NoteToggle)
        .value("NoteVelocity", Routing::MidiSource::Event::NoteVelocity)
        .value("NoteRange", Routing::MidiSource::Event::NoteRange)
        .export_values()
    ;

    py::class_<Routing::Route> route(routing, "Route");
    route
        .def_property("target", &Routing::Route::target, &Routing::Route::setTarget)
        .def_property("tracks", &Routing::Route::tracks, &Routing::Route::setTracks)
        .def("toggleTrack", &Routing::Route::toggleTrack, "trackIndex"_a)
        .def_property("min", &Routing::Route::min, &Routing::Route::setMin)
        .def_property("max", &Routing::Route::max, &Routing::Route::setMax)
        .def_property("source", &Routing::Route::source, &Routing::Route::setSource)
        .def_property_readonly("cvSource", [] (Routing::Route &route) { return &route.cvSource(); })
        .def_property_readonly("midiSource", [] (Routing::Route &route) { return &route.midiSource(); })
        .def("clear", &Routing::Route::clear)
    ;

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
