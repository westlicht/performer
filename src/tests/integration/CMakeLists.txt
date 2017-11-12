
function(register_test test file)
add_executable(${test} ${file})
target_link_libraries(${test} core)
platform_postprocess_executable(${test})
endfunction(register_test)

register_test(TestButtonLedMatrix drivers/TestButtonLedMatrix.cpp)
register_test(TestADC drivers/TestADC.cpp)
register_test(TestDAC drivers/TestDAC.cpp)
register_test(TestLCD drivers/TestLCD.cpp)
register_test(TestMIDI drivers/TestMIDI.cpp)
register_test(TestSDCard drivers/TestSDCard.cpp)

register_test(TestFileSystem fs/TestFileSystem.cpp)
