#pragma once

#include "USBMIDI.h"

#include <cstdint>

class USBH {
public:
    USBH(USBMIDI &usbMidi);

    void init();

    void process();

    void powerOn();
    void powerOff();
    bool powerFault();

private:
    void midiConnectDevice(uint8_t device) {
        _midiDevices |= (1 << device);
    }

    void midiDisconnectDevice(uint8_t device) {
        _midiDevices &= ~(1 << device);
    }

    bool midiDeviceConnected(uint8_t device) {
        return _midiDevices & (1 << device);
    }

    void midiEnqueueMessage(uint8_t device, MidiMessage &message) {
        _usbMidi.enqueueMessage(message);
    }

    void midiEnqueueData(uint8_t device, uint8_t data) {
        _usbMidi.enqueueData(data);
    }

    bool midiDequeueMessage(uint8_t *device, MidiMessage *message) {
        *device = 0;
        return _usbMidi.dequeueMessage(message);
    }

    USBMIDI &_usbMidi;

    uint8_t _midiDevices = 0;

    friend struct MIDIDriverHandler;
};
