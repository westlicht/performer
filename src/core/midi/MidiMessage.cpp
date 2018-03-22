#include "MidiMessage.h"

#include "core/Debug.h"

void MidiMessage::dump(const MidiMessage &msg) {
    if (msg.isChannelMessage()) {
        switch (msg.channelMessage()) {
        case NoteOff:           DBG("NoteOff(channel=%d, note=%d, velocity=%d)", msg.channel(), msg.note(), msg.velocity()); return;
        case NoteOn:            DBG("NoteOn(channel=%d, note=%d, velocity=%d)", msg.channel(), msg.note(), msg.velocity()); return;
        case KeyPressure:       DBG("KeyPressure(channel=%d, note=%d, pressure=%d)", msg.channel(), msg.note(), msg.keyPressure()); return;
        case ControlChange:     DBG("ControlChange(channel=%d, number=%d, value=%d)", msg.channel(), msg.controllerNumber(), msg.controllerValue()); return;
        case ProgramChange:     DBG("ProgramChange(channel=%d, number=%d)", msg.channel(), msg.programNumber()); return;
        case ChannelPressure:   DBG("ChannelPressure(channel=%d, pressure=%d", msg.channel(), msg.channelPressure()); return;
        case PitchBend:         DBG("PitchBend(channel=%d, value=%d", msg.channel(), msg.pitchBend()); return;
        }
    } else if (msg.isSystemMessage()) {
        switch (msg.systemMessage()) {
        case SystemExclusive:   DBG("SystemExclusive()"); return;
        case TimeCode:          DBG("TimeCode(data=%02x)", msg.data0()); return;
        case SongPosition:      DBG("SongPosition(position=%d)", msg.songPosition()); return;
        case SongSelect:        DBG("SongSelect(number=%d", msg.songNumber()); return;
        case TuneRequest:       DBG("TuneRequest()"); return;
        default: break;
        }
    } else if (msg.isRealTimeMessage()) {
        switch (msg.realTimeMessage()) {
        case Tick:              DBG("Tick"); return;
        case Start:             DBG("Start"); return;
        case Continue:          DBG("Continue"); return;
        case Stop:              DBG("Stop"); return;
        case ActiveSensing:     DBG("ActiveSensing"); return;
        case Reset:             DBG("Reset"); return;
        default: break;
        }
    }
    DBG("Unknown(status: %02x, data0: %02x, data1: %02x)", msg.status(), msg.data0(), msg.data1());
}
