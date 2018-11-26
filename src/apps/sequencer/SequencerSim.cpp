#include "SequencerApp.h"

#include "sim/Simulator.h"
#include "sim/frontend/Frontend.h"

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

    sim::Frontend frontend(sim);

    return frontend.main(argc, argv);
}
