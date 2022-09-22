enum ProjectVersion {
    // added NoteTrack::cvUpdateMode
    Version4 = 4,

    // added storing user scales with project
    // added Project::name
    // added UserScale::name
    Version5 = 5,

    // added Project::cvGateInput
    Version6 = 6,

    // added NoteSequence::Step::gateOffset
    Version7 = 7,

    // added CurveTrack::slideTime
    Version8 = 8,

    // added MidiCvTrack::arpeggiator
    Version9 = 9,

    // expanded divisor to 16 bits
    Version10 = 10,

    // added ClockSetup::clockOutputSwing
    // added Project::curveCvInput
    Version11 = 11,

    // added TrackState::fillAmount
    // added NoteSequence::Step::condition
    Version12 = 12,

    // added Routing::MidiSource::Event::NoteRange
    Version13 = 13,

    // swapped Curve::Type::Low with Curve::Type::High
    Version14 = 14,

    // added MidiCvTrack::lowNote/highNote
    // changed CurveSequence::Step layout
    // added CurveTrack::shapeProbabilityBias
    // added CurveTrack::gateProbabilityBias
    Version15 = 15,

    // added MidiCvTrack::notePriority
    Version16 = 16,

    // changed Arpeggiator::octaves
    Version17 = 17,

    // added Project::timeSignature
    Version18 = 18,

    // expanded Song::slots to 64 entries
    Version19 = 19,

    // added MidiCvTrack::slideTime
    Version20 = 20,

    // added MidiCvTrack::transpose
    Version21 = 21,

    // added CurveTrack::muteMode
    Version22 = 22,

    // added Route::Target::Scale and Route::Target::RootNote
    Version23 = 23,

    // expanded MidiOutput::outputs to 16 entries
    Version24 = 24,

    // added Song::Slot::mutes
    Version25 = 25,

    // added NoteTrack::fillMuted
    Version26 = 26,

    // expanded NoteSequence::Step to 64 bits, expanded NoteSequence::Step::Condition to 7 bits
    Version27 = 27,

    // added CurveTrack::offset
    Version28 = 28,

    // added Project::midiInput
    Version29 = 29,

    // added Project::monitorMode
    Version30 = 30,

    // changed MidiCvTrack::VoiceConfig to 8-bit value
    Version31 = 31,

    // added Project::midiIntegrationMode, Project::midiProgramOffset, Project::alwaysSync
    Version32 = 32,

    // automatically derive latest version
    Last,
    Latest = Last - 1,
};
