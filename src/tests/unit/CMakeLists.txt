
include_directories(../../test)

function(register_test test file)
    add_executable(${test} ${file})
    target_link_libraries(${test} core)
    platform_postprocess_executable(${test})
    add_test(NAME ${test} COMMAND ${test})
endfunction(register_test)

add_subdirectory(core)
add_subdirectory(sequencer)
