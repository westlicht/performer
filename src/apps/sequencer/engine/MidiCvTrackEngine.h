#pragma once

#include "TrackEngine.h"

#include "model/Track.h"

class MidiCvTrackEngine : public TrackEngine {
public:
    MidiCvTrackEngine(const Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(track, linkedTrackEngine),
        _midiCvTrack(track.midiCvTrack())
    {
    }

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void update(float dt) override;
    virtual void receiveMidi(MidiPort port, int channel, const MidiMessage &message) override;

    virtual bool activity() const override;
    virtual bool gateOutput(int index) const override;
    virtual float cvOutput(int index) const override;

    static constexpr Track::TrackMode trackMode = Track::TrackMode::MidiCv;

private:
    float noteToCv(int note) const;
    float valueToCv(int value) const;
    float pitchBendToCv(int value) const;

    struct Voice;

    void resetVoices();
    Voice *allocateVoice(int note, int numVoices);
    void freeVoice(int note, int numVoices);

    Voice *findVoice(int begin, int end, int note);
    Voice *lruVoice(int begin, int end);
    Voice *mruVoice(int begin, int end);

    void printVoices();

    void updateActivity();

    static constexpr int RetriggerDelay = 2;

    const MidiCvTrack &_midiCvTrack;

    struct Voice {
        uint32_t ticks = 0;
        uint8_t note;
        float pitchCv;
        float velocityCv;
        float pressureCv;
    };

    std::array<Voice, 8> _voices;

    bool _activity;

    float _pitchBendCv;
    float _channelPressureCv;
};
