#include "sim/Simulator.h"
#include "SequencerApp.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

void register_core(py::module &m);
void register_simulator(py::module &m);
void register_sequencer(py::module &m);

struct Environment {
    Environment() {
        simulator.reset(new sim::Simulator({
            .create = [this] () {
                sequencer.reset(new SequencerApp());
            },
            .destroy = [this] () {
                sequencer.reset();
            },
            .update = [this] () {
                sequencer->update();
            }
        }));
    }

    std::unique_ptr<SequencerApp> sequencer;
    std::unique_ptr<sim::Simulator> simulator;
};

PYBIND11_MODULE(testsim, m) {
    m.doc() = "performer testing simulator";

    py::module m_core = m.def_submodule("core");
    py::module m_simulator = m.def_submodule("simulator");
    py::module m_sequencer = m.def_submodule("sequencer");

    register_core(m_core);
    register_simulator(m_simulator);
    register_sequencer(m_sequencer);

    // ------------------------------------------------------------------------
    // Environment
    // ------------------------------------------------------------------------

    py::class_<Environment> environment(m, "Environment", py::dynamic_attr());
    environment
        .def(py::init<>())

        .def_property_readonly("simulator", [] (Environment &env) {
            return env.simulator.get();
        })
        .def_property_readonly("sequencer", [] (Environment &env) {
            return env.sequencer.get();
        })
    ;
}
