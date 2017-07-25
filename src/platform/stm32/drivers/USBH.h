#pragma once

class USBH {
public:
    void init();

    void process();

    int mouseX() const { return _mouseX; }
    int mouseY() const { return _mouseY; }

    void setMousePos(int x, int y) { _mouseX = x; _mouseY = y; }

private:
    int _mouseX = 0;
    int _mouseY = 0;
};