#include "GateOutput.h"

GateOutput::GateOutput(ShiftRegister &shiftRegister) :
    _shiftRegister(shiftRegister)
{}

void GateOutput::init() {
}

void GateOutput::update() {
    _shiftRegister.write(2, _gates);
}
