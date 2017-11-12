
include_directories(../../libs/lest/include/lest)

function(register_test test file)
    add_executable(${test} ${file})
    platform_postprocess_executable(${test})
    add_test(NAME ${test} COMMAND ${test})
endfunction(register_test)

add_subdirectory(core)
