#include "SequencerApp.h"

#include "sim/Simulator.h"

#include <memory>

int main(int argc, char *argv[]) {
    std::unique_ptr<SequencerApp> app;

    sim::Simulator sim({
        .create = [&] () {
            app.reset(new SequencerApp());
        },
        .destroy = [&] () {
            app.reset();
        },
        .update = [&] () {
            app->update();
        }
    });

    sim.run();

    return 0;
}
