
pybind11_add_module(testsim testsim.cpp core.cpp project.cpp sequencer.cpp simulator.cpp)
target_link_libraries(testsim PRIVATE sequencer_shared)
