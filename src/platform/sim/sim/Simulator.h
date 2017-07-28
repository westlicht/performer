#pragma once

#include "Common.h"
#include "Window.h"
#include "Audio.h"
#include "DrumKit.h"

namespace sim {

class Simulator {
public:
    typedef std::function<void()> UpdateCallback;

    Simulator();

    Window &window() { return _window; }
    Audio &audio() { return _audio; }

    bool terminate() const;

    void update();
    void render();

    double ticks();

    void writeGate(int channel, bool value);
    void writeDAC(int channel, uint16_t value);
    uint16_t readADC(int channel);

    void addUpdateCallback(UpdateCallback callback);

private:
    Window _window;
    Audio _audio;
    DrumKit _drumKit;

    double _timerFrequency;
    double _timerStart;

    std::array<bool, 8> _gate;

    std::vector<UpdateCallback> _updateCallbacks;
};

} // namespace sim
