#include "ADC.h"

#include <cstdint>

ADC::ADC() :
    _simulator(sim::Simulator::instance())
{
}
