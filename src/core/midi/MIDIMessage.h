#pragma once

class MIDIMessage {
public:
    enum RealTimeMessage {
        RealTimeTick            = 0xf8,
        RealTimeStart           = 0xfa,
        RealTimeContinue        = 0xfb,
        RealTimeStop            = 0xfc,
        RealTimeActiveSensing   = 0xfe,
        RealTimeReset           = 0xff,
    };

    static bool isRealTimeMessage(uint8_t msg) {
        return (msg & 0xf8) == 0xf8;
    }

    static bool isClockMessage(uint8_t msg) {
        return isRealTimeMessage(msg) && (
            msg == RealTimeTick ||
            msg == RealTimeStart ||
            msg == RealTimeContinue ||
            msg == RealTimeStop
        );
    }

};
