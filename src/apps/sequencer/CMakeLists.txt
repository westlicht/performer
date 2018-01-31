
set(sources
    # engine
    engine/Engine.cpp
    engine/Clock.cpp
    engine/Controller.cpp
    engine/Track.cpp
    engine/controllers/LaunchpadController.cpp
    # ui
    ui/LedPainter.cpp
    ui/Page.cpp
    ui/PageManager.cpp
    ui/UI.cpp
    # ui/pages
    ui/pages/ClockSetupPage.cpp
    ui/pages/MainPage.cpp
    ui/pages/MenuPage.cpp
    ui/pages/MutePage.cpp
    ui/pages/SequenceSetupPage.cpp
    ui/pages/TopPage.cpp
    ui/pages/TrackPage.cpp
    ui/pages/ValuePage.cpp
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
