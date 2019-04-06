#pragma once

#include "LaunchpadDevice.h"

#include "core/midi/MidiMessage.h"

// Compatible with Launchpad Pro
class LaunchpadProDevice : public LaunchpadDevice {
public:
    LaunchpadProDevice();

    void recvMidi(const MidiMessage &message) override;

    void setLed(int row, int col, Color color) override {
        _ledState[row * Cols + col] = mapColor(color.red, color.green);
    }

    void setLed(int row, int col, int red, int green) override {
        _ledState[row * Cols + col] = mapColor(red, green);;
    }

    void syncLeds() override;

private:
    inline uint8_t mapColor(int red, int green) const {
        static const uint8_t map[] = {
        //  g0 g1 g2 g3
            0, 23, 22, 21, // r0
            7, 15, 18, 21, // r1
            6, 10, 14, 17, // r2
            5,  5,  9, 13, // r3
        };
        return map[(red & 0x3) * 4 + (green & 0x3)];
    }
};
