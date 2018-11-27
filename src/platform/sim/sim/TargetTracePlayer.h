#pragma once

#include "Target.h"
#include "TargetTrace.h"

#include <vector>
#include <memory>

namespace sim {

class Simulator;

struct TracePlayerBase;

class TargetTracePlayer : public TargetTickHandler {
public:
    TargetTracePlayer(const TargetTrace &targetTrace, TargetInputHandler *targetInputHandler, TargetOutputHandler *targetOutputHandler);
    ~TargetTracePlayer();

    const TargetTrace &targetTrace() const { return _targetTrace; }

protected:
    virtual void setTick(uint32_t tick) override;

    const TargetTrace &_targetTrace;
    TargetInputHandler *_targetInputHandler;
    TargetOutputHandler *_targetOutputHandler;

    std::vector<std::unique_ptr<TracePlayerBase>> _tracePlayers;
};

} // namespace sim
