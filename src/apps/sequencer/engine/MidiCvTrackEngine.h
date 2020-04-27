#pragma once

#include "TrackEngine.h"
#include "ArpeggiatorEngine.h"

#include "model/Track.h"

class MidiCvTrackEngine : public TrackEngine {
public:
    MidiCvTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _midiCvTrack(track.midiCvTrack()),
        _arpeggiatorEngine(_midiCvTrack.arpeggiator())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::MidiCv; }

    virtual void reset() override;
    virtual void restart() override;
    virtual TickResult tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual bool receiveMidi(MidiPort port, const MidiMessage &message) override;

    virtual bool activity() const override;
    virtual bool gateOutput(int index) const override;
    virtual float cvOutput(int index) const override;

private:
    static constexpr size_t VoiceCount = 8;
    static constexpr int RetriggerDelay = 2;

    struct Voice {
        uint32_t ticks = 0;
        uint8_t note = 60;
        uint8_t velocity = 0;
        uint8_t pressure = 0;
        int8_t output = -1;

        void reset() {
            ticks = 0;
            output = -1;
        }

        void release() { ticks = 0; }

        bool isActive() const { return ticks != 0; }
        bool isAllocated() const { return output != -1; }
    };

    void updateActivity();

    void updateArpeggiator();
    void tickArpeggiator(uint32_t tick);

    float noteToCv(int note) const;
    float valueToCv(int value) const;
    float pitchBendToCv(int value) const;

    void resetVoices();

    void addVoice(int note, int velocity);
    void removeVoice(int note);
    Voice *findVoice(int note);

    void sortVoices();
    void printVoices();

    const MidiCvTrack &_midiCvTrack;

    ArpeggiatorEngine _arpeggiatorEngine;
    bool _arpeggiatorEnabled;
    float _arpeggiatorTime;
    uint32_t _arpeggiatorTick;

    std::array<Voice, VoiceCount> _voices;
    std::array<int8_t, VoiceCount> _voiceByOutput;
    int8_t _nextOutput;

    bool _activity;

    int16_t _pitchBend;
    uint8_t _channelPressure;

    // slides for pitch, only valid in monophonic mode
    bool _slideActive;
    float _pitchCvOutputTarget;
    float _pitchCvOutput;
};
