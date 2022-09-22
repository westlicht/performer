set(update_file "UPDATE.DAT")

set(sources
    # asteroids
    asteroids/Asteroids.cpp
    # engine
    engine/ArpeggiatorEngine.cpp
    engine/Clock.cpp
    engine/CurveTrackEngine.cpp
    engine/CvInput.cpp
    engine/CvOutput.cpp
    engine/Engine.cpp
    engine/MidiCvTrackEngine.cpp
    engine/MidiLearn.cpp
    engine/MidiOutputEngine.cpp
    engine/NoteTrackEngine.cpp
    engine/RoutingEngine.cpp
    engine/SequenceState.cpp
    # engine/generators
    engine/generators/EuclideanGenerator.cpp
    engine/generators/Generator.cpp
    engine/generators/RandomGenerator.cpp
    engine/generators/Rhythm.cpp
    # intro
    intro/Intro.cpp
    # model
    model/Arpeggiator.cpp
    model/Calibration.cpp
    model/ClipBoard.cpp
    model/ClockSetup.cpp
    model/Curve.cpp
    model/CurveSequence.cpp
    model/CurveTrack.cpp
    model/FileManager.cpp
    model/MidiCvTrack.cpp
    model/MidiOutput.cpp
    model/Model.cpp
    model/ModelUtils.cpp
    model/NoteSequence.cpp
    model/NoteTrack.cpp
    model/PlayState.cpp
    model/Project.cpp
    model/Routing.cpp
    model/Scale.cpp
    model/Settings.cpp
    model/Song.cpp
    model/TimeSignature.cpp
    model/Track.cpp
    model/Types.cpp
    model/UserScale.cpp
    model/UserSettings.cpp
    # ui
    ui/Controller.cpp
    ui/ControllerManager.cpp
    ui/LedPainter.cpp
    ui/MessageManager.cpp
    ui/Page.cpp
    ui/PageManager.cpp
    ui/Ui.cpp
    ui/Screensaver.cpp
    # ui/controllers/launchpad
    ui/controllers/launchpad/LaunchpadController.cpp
    ui/controllers/launchpad/LaunchpadDevice.cpp
    ui/controllers/launchpad/LaunchpadMk2Device.cpp
    ui/controllers/launchpad/LaunchpadMk3Device.cpp
    ui/controllers/launchpad/LaunchpadProDevice.cpp
    ui/controllers/launchpad/LaunchpadProMk3Device.cpp
    # ui/pages
    ui/pages/AsteroidsPage.cpp
    ui/pages/BasePage.cpp
    ui/pages/BusyPage.cpp
    ui/pages/ClockSetupPage.cpp
    ui/pages/ConfirmationPage.cpp
    ui/pages/ContextMenuPage.cpp
    ui/pages/CurveSequenceEditPage.cpp
    ui/pages/CurveSequencePage.cpp
    ui/pages/FileSelectPage.cpp
    ui/pages/GeneratorPage.cpp
    ui/pages/GeneratorSelectPage.cpp
    ui/pages/IntroPage.cpp
    ui/pages/LayoutPage.cpp
    ui/pages/ListPage.cpp
    ui/pages/MidiOutputPage.cpp
    ui/pages/MonitorPage.cpp
    ui/pages/NoteSequenceEditPage.cpp
    ui/pages/NoteSequencePage.cpp
    ui/pages/OverviewPage.cpp
    ui/pages/PatternPage.cpp
    ui/pages/PerformerPage.cpp
    ui/pages/ProjectPage.cpp
    ui/pages/QuickEditPage.cpp
    ui/pages/RoutingPage.cpp
    ui/pages/SongPage.cpp
    ui/pages/StartupPage.cpp
    ui/pages/SystemPage.cpp
    ui/pages/TempoPage.cpp
    ui/pages/TextInputPage.cpp
    ui/pages/TopPage.cpp
    ui/pages/TrackPage.cpp
    ui/pages/UserScalePage.cpp
    # ui/painters
    ui/painters/SequencePainter.cpp
    ui/painters/SongPainter.cpp
    ui/painters/WindowPainter.cpp
)

include_directories(.)

if(${PLATFORM} STREQUAL "stm32")
    add_library(sequencer_shared ${sources})
    target_link_libraries(sequencer_shared core)

    add_executable(sequencer Sequencer.cpp)
    target_link_libraries(sequencer sequencer_shared)
    platform_postprocess_executable(sequencer)
    # override linker script
    set_target_properties(sequencer PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/sequencer.ld")

    # create update file
    add_custom_command(TARGET sequencer POST_BUILD COMMAND ${CMAKE_SOURCE_DIR}/scripts/makeupdate sequencer.bin ${update_file})

    add_executable(sequencer_standalone Sequencer.cpp)
    target_link_libraries(sequencer_standalone sequencer_shared)
    platform_postprocess_executable(sequencer_standalone)
    # override linker script
    set_target_properties(sequencer_standalone PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/sequencer_standalone.ld")
endif()

if(${PLATFORM} STREQUAL "sim")
    add_library(sequencer_shared ${sources})
    target_link_libraries(sequencer_shared core)

    add_executable(sequencer SequencerSim.cpp)
    target_link_libraries(sequencer sequencer_shared)
    platform_postprocess_executable(sequencer)
    add_custom_command(TARGET sequencer COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/sim/assets ${CMAKE_BINARY_DIR}/assets)

    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        add_subdirectory(python)
    endif()
endif()
