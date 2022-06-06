#include "MidiMessage.h"

#include "core/Debug.h"

MidiMessage::PayloadPool MidiMessage::_payloadPool;

void MidiMessage::dump(const MidiMessage &msg) {
    if (msg.isChannelMessage()) {
        switch (msg.channelMessage()) {
        case NoteOff:           DBG("NoteOff(channel=%d, note=%d, velocity=%d)", msg.channel(), msg.note(), msg.velocity()); return;
        case NoteOn:            DBG("NoteOn(channel=%d, note=%d, velocity=%d)", msg.channel(), msg.note(), msg.velocity()); return;
        case KeyPressure:       DBG("KeyPressure(channel=%d, note=%d, pressure=%d)", msg.channel(), msg.note(), msg.keyPressure()); return;
        case ControlChange:     DBG("ControlChange(channel=%d, number=%d, value=%d)", msg.channel(), msg.controlNumber(), msg.controlValue()); return;
        case ProgramChange:     DBG("ProgramChange(channel=%d, number=%d)", msg.channel(), msg.programNumber()); return;
        case ChannelPressure:   DBG("ChannelPressure(channel=%d, pressure=%d)", msg.channel(), msg.channelPressure()); return;
        case PitchBend:         DBG("PitchBend(channel=%d, value=%d)", msg.channel(), msg.pitchBend()); return;
        }
    } else if (msg.isSystemMessage()) {
        switch (msg.systemMessage()) {
        case SystemExclusive:   DBG("SystemExclusive"); return;
        case TimeCode:          DBG("TimeCode(data=%02x)", msg.data0()); return;
        case SongPosition:      DBG("SongPosition(position=%d)", msg.songPosition()); return;
        case SongSelect:        DBG("SongSelect(number=%d)", msg.songNumber()); return;
        case TuneRequest:       DBG("TuneRequest"); return;
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

void MidiMessage::setPayloadPool(uint8_t *data, size_t length) {
    _payloadPool.data = data;
    _payloadPool.length = length;
}

MidiMessage::PayloadID MidiMessage::allocatePayload(size_t length) {
    if (!_payloadPool.valid()) {
        return InvalidPayload;
    }

    const size_t slotLength = _payloadPool.length / PayloadPool::SlotCount;
    ASSERT(length <= slotLength, "Requested length does not fit.");
    if (length > slotLength) {
        return InvalidPayload;
    }

    for (size_t slotIndex = 0; slotIndex < _payloadPool.slots.size(); ++slotIndex) {
        auto &slot = _payloadPool.slots[slotIndex];
        if (!slot.data) {
            slot.data = _payloadPool.data + slotIndex * slotLength;
            slot.length = length;
            slot.refCount = 1;
            // DBG("allocate payload: id=%d, refCount=%d, length=%zd", _payloadPool.payloadID, _payloadPool.payloadRefCount, _payloadPool.payloadLength);
            return PayloadID(slotIndex + 1);
        }
    }
    return InvalidPayload;
}

void MidiMessage::incPayloadRefCount(PayloadID id) {
    auto slot = _payloadPool.getSlot(id);
    if (slot) {
        slot->refCount++;
        // DBG("inc refcount: id=%d, refCount=%d, length=%zd", _payloadPool.payloadID, _payloadPool.payloadRefCount, _payloadPool.payloadLength);
    }
}

void MidiMessage::decPayloadRefCount(PayloadID id) {
    auto slot = _payloadPool.getSlot(id);
    if (slot) {
        slot->refCount--;
        // DBG("dec refcount: id=%d, refCount=%d, length=%zd", _payloadPool.payloadID, _payloadPool.payloadRefCount, _payloadPool.payloadLength);
        if (slot->refCount == 0) {
            slot->data = nullptr;
            // DBG("free payload: id=%d, refCount=%d, length=%zd", _payloadPool.payloadID, _payloadPool.payloadRefCount, _payloadPool.payloadLength);
        }
    }
}

uint8_t *MidiMessage::payloadData(PayloadID id) {
    auto slot = _payloadPool.getSlot(id);
    return slot ? slot->data : nullptr;
}

size_t MidiMessage::payloadLength(PayloadID id) {
    auto slot = _payloadPool.getSlot(id);
    return slot ? slot->length : 0;
}
