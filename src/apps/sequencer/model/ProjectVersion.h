namespace ProjectVersion {

    // added NoteTrack::cvUpdateMode
    static constexpr uint32_t Version4 = 4;

    // added storing user scales with project
    // added Project::name
    // added UserScale::name
    static constexpr uint32_t Version5 = 5;

    // added Project::cvGateInput
    static constexpr uint32_t Version6 = 6;

    // added NoteSequence::Step::gateOffset
    static constexpr uint32_t Version7 = 7;

    // added CurveTrack::slideTime
    static constexpr uint32_t Version8 = 8;

    // added MidiCvTrack::arpeggiator
    static constexpr uint32_t Version9 = 9;

    // expanded divisor to 16 bits
    static constexpr uint32_t Version10 = 10;

    // added ClockSetup::clockOutputSwing
    // added Project::curveCvInput
    static constexpr uint32_t Version11 = 11;

    // added TrackState::fillAmount
    // added NoteSequence::Step::condition
    static constexpr uint32_t Version12 = 12;

    // added Routing::MidiSource::Event::NoteRange
    static constexpr uint32_t Version13 = 13;

    static constexpr uint32_t Version = Version13;

} // namespace ProjectVersion
