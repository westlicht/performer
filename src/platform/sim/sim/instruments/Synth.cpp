#include "Synth.h"

namespace sim {

class Oscillator {
public:
    enum Waveform {
        Sine,
    };

    Oscillator(float sampleRate) :
        _sampleRate(sampleRate)
    {}

    Waveform waveform() const { return _waveform; }
    void setWaveform(Waveform waveform) { _waveform = waveform; }

    float frequency() const { return _frequency; }
    void setFrequency(float frequency) {
        _frequency = frequency;
        _increment = frequency / _sampleRate;
    }

    inline float process() {
        float result = 0.f;
        switch (_waveform) {
        case Sine:
            result = std::sin(TWO_PI * _phase);
            break;
        }
        _phase = std::fmod(_phase + _increment, 1.f);
        return result;
    }

private:
    Waveform _waveform = Sine;
    float _sampleRate;
    float _frequency;
    float _phase = 0.f;
    float _increment = 0.f;
};

class Voice {
public:
    Voice(float sampleRate) :
        _osc(sampleRate)
    {}

    void setGate(bool gate) {
        _env = gate ? 1.f : 0.f;
    }

    void setCV(float cv) {
        _osc.setFrequency(BaseFrequency * std::exp2(cv));
    }

    inline float process() {
        return _env * _osc.process();
    }

private:
    static constexpr float BaseFrequency = 440.f / 4.f;

    Oscillator _osc;
    float _env = 0.f;
};



SynthInstance::SynthInstance(Synth &synth) :
    _synth(synth)
{
    _voice.reset(new Voice(mBaseSamplerate));
}

SynthInstance::~SynthInstance() {
}

void SynthInstance::getAudio(float *aBuffer, unsigned int aSamples) {
    for (size_t i = 0; i < aSamples; ++i) {
        if (i % 128 == 0) {
            _voice->setGate(_synth._gate);
            _voice->setCV(_synth._cv);
        }
        aBuffer[i] = _voice->process();
    }
}

bool SynthInstance::hasEnded() {
    return false;
}



Synth::Synth(Audio &audio) :
    _audio(audio)
{
    setSingleInstance(true);
    _audio.engine().play(*this);
}

SoLoud::AudioSourceInstance *Synth::createInstance() {
    return new SynthInstance(*this);
}

void Synth::setGate(bool gate) {
    _gate = gate;
}

void Synth::setCV(float cv) {
    _cv = cv;
}

} // namespace sim
