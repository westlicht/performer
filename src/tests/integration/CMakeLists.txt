
include_directories(../../test)

function(register_test test file)
    add_executable(${test} ${file})
    target_link_libraries(${test} core)
    platform_postprocess_executable(${test})
endfunction(register_test)

register_test(TestButtonLedMatrix drivers/TestButtonLedMatrix.cpp)
register_test(TestAdc drivers/TestAdc.cpp)
register_test(TestDac drivers/TestDac.cpp)
register_test(TestDio drivers/TestDio.cpp)
register_test(TestEncoder drivers/TestEncoder.cpp)
register_test(TestGateOutput drivers/TestGateOutput.cpp)
register_test(TestLcd drivers/TestLcd.cpp)
register_test(TestMidi drivers/TestMidi.cpp)
register_test(TestSdCard drivers/TestSdCard.cpp)
register_test(TestUsbMidi drivers/TestUsbMidi.cpp)

register_test(TestFileSystem fs/TestFileSystem.cpp)
