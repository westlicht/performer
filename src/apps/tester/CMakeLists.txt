set(sources
    Tester.cpp
)

if(${PLATFORM} STREQUAL "stm32")
    add_executable(tester ${sources})
    target_link_libraries(tester core)
    target_linker_script(tester tester.ld)
    platform_postprocess_executable(tester)

    add_executable(tester_standalone ${sources})
    target_link_libraries(tester_standalone core)
    target_linker_script(tester_standalone tester_standalone.ld)
    platform_postprocess_executable(tester_standalone)
endif()
