#pragma once

class EngineState {
public:
    bool running() const { return _running; }
    bool recording() const { return _recording; }

private:
    void setRunning(bool running) { _running = running; }
    void setRecording(bool recording) { _recording = recording; }

    bool _running = false;
    bool _recording = false;

    friend class Engine;
};
