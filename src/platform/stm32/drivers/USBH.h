#pragma once

#include "USBMIDI.h"

class USBH {
public:
    USBH(USBMIDI &usbMidi);

    void init();

    void process();

    void powerOn();
    void powerOff();
    bool powerFault();

    int mouseX() const { return _mouseX; }
    int mouseY() const { return _mouseY; }

    void setMousePos(int x, int y) { _mouseX = x; _mouseY = y; }

private:
    USBMIDI &_usbMidi;

    int _mouseX = 0;
    int _mouseY = 0;
};
