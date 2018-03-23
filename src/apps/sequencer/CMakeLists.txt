
set(sources
    # asteroids
    asteroids/Asteroids.cpp
    # engine
    engine/Clock.cpp
    engine/Controller.cpp
    engine/Curve.cpp
    engine/CurveTrackEngine.cpp
    engine/CvInput.cpp
    engine/CvOutput.cpp
    engine/Engine.cpp
    engine/MidiCvTrackEngine.cpp
    engine/NoteTrackEngine.cpp
    engine/RoutingEngine.cpp
    engine/Scale.cpp
    engine/SequenceState.cpp
    engine/TrackEngine.cpp
    # engine/controllers
    engine/controllers/LaunchpadController.cpp
    # engine/generators
    engine/generators/Rhythm.cpp
    # model
    model/ClipBoard.cpp
    model/ClockSetup.cpp
    model/CurveSequence.cpp
    model/CurveTrack.cpp
    model/MidiCvTrack.cpp
    model/Model.cpp
    model/ModelUtils.cpp
    model/NoteSequence.cpp
    model/NoteTrack.cpp
    model/PlayState.cpp
    model/Project.cpp
    model/ProjectManager.cpp
    model/Routing.cpp
    model/Settings.cpp
    model/Track.cpp
    # ui
    ui/LedPainter.cpp
    ui/MessageManager.cpp
    ui/Page.cpp
    ui/PageManager.cpp
    ui/Ui.cpp
    # ui/pages
    ui/pages/AsteroidsPage.cpp
    ui/pages/BpmPage.cpp
    ui/pages/BusyPage.cpp
    ui/pages/ClockSetupPage.cpp
    ui/pages/ConfirmationPage.cpp
    ui/pages/ContextMenuPage.cpp
    ui/pages/CurveSequencePage.cpp
    ui/pages/CurveSequenceSetupPage.cpp
    ui/pages/EuclideanPage.cpp
    ui/pages/LayoutPage.cpp
    ui/pages/ListPage.cpp
    ui/pages/MonitorPage.cpp
    ui/pages/NoteSequencePage.cpp
    ui/pages/NoteSequenceSetupPage.cpp
    ui/pages/PatternPage.cpp
    ui/pages/PerformerPage.cpp
    ui/pages/ProjectPage.cpp
    ui/pages/ProjectSelectPage.cpp
    ui/pages/TextInputPage.cpp
    ui/pages/TopPage.cpp
    ui/pages/TrackSetupPage.cpp
    # ui/painters
    ui/painters/SequencePainter.cpp
    ui/painters/WindowPainter.cpp
)

include_directories(.)

if(${PLATFORM} MATCHES "stm32")
    add_executable(sequencer Sequencer.cpp ${sources})
    target_link_libraries(sequencer core)
    platform_postprocess_executable(sequencer)
endif()

if(${PLATFORM} MATCHES "sim")
    add_executable(sequencer SequencerSim.cpp ${sources})
    target_link_libraries(sequencer core)
    platform_postprocess_executable(sequencer)
    add_custom_command(TARGET sequencer COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/sim/assets ${CMAKE_BINARY_DIR}/assets)
endif()
