
set(sources
    # asteroids
    asteroids/Asteroids.cpp
    # engine
    engine/Clock.cpp
    engine/Controller.cpp
    engine/Curve.cpp
    engine/CVInput.cpp
    engine/CVOutput.cpp
    engine/Engine.cpp
    engine/Scale.cpp
    engine/Track.cpp
    # engine/controllers
    engine/controllers/LaunchpadController.cpp
    # model
    model/ClockSetup.cpp
    model/CurveSequence.cpp
    model/Model.cpp
    model/NoteSequence.cpp
    model/Pattern.cpp
    model/PlayState.cpp
    model/Project.cpp
    model/Sequence.cpp
    model/Settings.cpp
    model/TrackSetup.cpp
    # ui
    ui/LedPainter.cpp
    ui/MessageManager.cpp
    ui/Page.cpp
    ui/PageManager.cpp
    ui/UI.cpp
    # ui/pages
    ui/pages/AsteroidsPage.cpp
    ui/pages/BpmPage.cpp
    ui/pages/BusyPage.cpp
    ui/pages/ClockSetupPage.cpp
    ui/pages/ConfirmationPage.cpp
    ui/pages/CurveSequencePage.cpp
    ui/pages/ListPage.cpp
    ui/pages/MonitorPage.cpp
    ui/pages/NoteSequencePage.cpp
    ui/pages/PatternPage.cpp
    ui/pages/PerformerPage.cpp
    ui/pages/ProjectPage.cpp
    ui/pages/SequenceSetupPage.cpp
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
