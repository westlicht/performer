#pragma once

#include "UsbMidi.h"

#include <cstdint>

class UsbH {
public:
    UsbH(UsbMidi &usbMidi);

    void init();

    void process();

    void powerOn();
    void powerOff();
    bool powerFault();

private:
    void midiConnectDevice(uint8_t device, uint16_t vendorId, uint16_t productId) {
        _midiDevices |= (1 << device);
        _usbMidi.connect(vendorId, productId);
    }

    void midiDisconnectDevice(uint8_t device) {
        _midiDevices &= ~(1 << device);
        _usbMidi.disconnect();
    }

    bool midiDeviceConnected(uint8_t device) {
        return _midiDevices & (1 << device);
    }

    void midiEnqueueMessage(uint8_t device, uint8_t cable, const MidiMessage &message) {
        _usbMidi.enqueueMessage(cable, message);
    }

    void midiEnqueueData(uint8_t device, uint8_t cable, uint8_t data) {
        _usbMidi.enqueueData(cable, data);
    }

    bool midiDequeueMessage(uint8_t *device, uint8_t *cable, MidiMessage *message) {
        *device = 0;
        return _usbMidi.dequeueMessage(cable, message);
    }

    UsbMidi &_usbMidi;

    uint8_t _midiDevices = 0;

    friend struct MidiDriverHandler;
};
