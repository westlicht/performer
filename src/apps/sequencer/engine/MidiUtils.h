#pragma once

#include "MidiPort.h"

#include "model/MidiConfig.h"

#include "core/midi/MidiMessage.h"

namespace MidiUtils {

static bool matchSource(MidiPort port, const MidiMessage &message, const MidiSourceConfig &source) {
    return port == MidiPort(source.port()) && (source.isOmni() || message.channel() == source.channel());
}

} // namespace MidiUtils
