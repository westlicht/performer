#pragma once

#include <memory>

namespace sim {

class Instrument {
public:
    typedef std::shared_ptr<Instrument> Ptr;

    virtual ~Instrument() {}

    virtual void setGate(bool gate) = 0;
    virtual void setCv(float cv) = 0;
};

} // namespace sim
