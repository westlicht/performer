#include "Synth.h"

#include <cstdint>
#include <cmath>

namespace sim {

static float flushDenormal(float value) {
    return ((((*(uint32_t *) &(value))) & 0x7f800000) == 0) ? 0.f : value;
}

class Oscillator {
public:
    enum Waveform {
        Sine,
        Triangle,
        Sawtooth,
        Square,
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
        case Triangle:
            result = 1.f - std::abs(_phase * 4.f - 2.f);
            break;
        case Sawtooth:
            result = _phase * 2.f - 1.f;
            break;
        case Square:
            result = _phase < 0.5f ? -1.f : 1.f;
            break;
        }

        _phase = std::fmod(_phase + _increment, 1.f);
        return result;
    }

private:
    float _sampleRate;
    Waveform _waveform = Sine;
    float _frequency = 100.f;
    float _phase = 0.f;
    float _increment = 0.f;
};

class Filter {
public:
    enum Mode {
        LowPass,
        HighPass,
        BandPass,
    };

    Filter(float sampleRate) :
        _invSampleRate(1.f / sampleRate)
    {
        setMode(LowPass);
        setFrequency(0.05f * sampleRate);
        setResonance(0.8f);
    }

    Mode mode() const { return _mode; }
    void setMode(Mode mode) {
        _mode = mode;
    }

    float frequency() const { return _frequency; }
    void setFrequency(float frequency) {
        _frequency = frequency;
        _g = std::tan(M_PI * std::max(0.f, std::min(1.f, _frequency * _invSampleRate)));

    }

    float resonance() const { return _resonance; }
    void setResonance(float resonance) {
        _resonance = std::max(0.f, std::min(1.f, resonance));
        _k = 2.f - 2.f * _resonance;
    }

    inline float process(float input) {
        float a1 = 1.f / (1.f + _g * (_g + _k));
        float a2 = _g * a1;
        float a3 = _g * a2;
        float m0 = 0.f, m1 = 0.f, m2 = 1.f;

        switch (_mode) {
        case LowPass:
            break;
        case HighPass:
            m0 = 1.f;
            m1 = -_k;
            m2 = -1.f;
            break;
        case BandPass:
            m1 = 1.f;
            m2 = 0.f;
            break;
        }

        flushDenormal(input);

        float v0 = input;
        float v3 = v0 - _ic2eq;
        float v1 = a1 * _ic1eq + a2 * v3;
        float v2 = _ic2eq + a2 * _ic1eq + a3 * v3;
        _ic1eq = 2.f * v1 - _ic1eq;
        _ic2eq = 2.f * v2 - _ic2eq;

        flushDenormal(_ic1eq);
        flushDenormal(_ic2eq);

        return m0 * v0 + m1 * v1 + m2 * v2;
    }

private:
    float _invSampleRate;
    Mode _mode;
    float _frequency;
    float _resonance;

    float _ic1eq = 0.f;
    float _ic2eq = 0.f;
    float _g;
    float _k;
};

class ADSR {
public:
    ADSR(float sampleRate) :
        _invSampleRate(1.f / sampleRate)
    {
        setAttack(0.01f);
        setDecay(0.f);
        setSustain(1.f);
        setRelease(0.1f);
    }

    float attack() const { return _attack; }
    float decay() const { return _decay; }
    float sustain() const { return _sustain; }
    float release() const { return _release; }

    void setAttack(float attack) { _attack = attack; _attackIncrement = _invSampleRate / _attack; }
    void setDecay(float decay) { _decay = decay; _decayIncrement = _invSampleRate / _decay; }
    void setSustain(float sustain) { _sustain = sustain; }
    void setRelease(float release) { _release = release; _releaseIncrement = _invSampleRate / _release; }

    void setGate(bool gate) {
        _gate = gate;
    }

    inline float process() {
        switch (_state) {
        case Idle:
            if (_gate) {
                _state = Attack;
            }
            break;
        case Attack:
            if (_gate) {
                _value += _attackIncrement;
                if (_value >= 1.f) {
                    _value = 1.f;
                    _state = Decay;
                }
            } else {
                _state = Release;
            }
            break;
        case Decay:
            if (_gate) {
                _value -= _decayIncrement;
                if (_value <= _sustain) {
                    _value = _sustain;
                    _state = Sustain;
                }
            } else {
                _state = Release;
            }
            break;
        case Sustain:
            if (!_gate) {
                _state = Release;
            }
            break;
        case Release:
            if (_gate) {
                _state = Attack;
            } else {
                _value -= _releaseIncrement;
                if (_value <= 0.f) {
                    _value = 0.f;
                    _state = Idle;
                }
            }
            break;
        }

        return _value;
    }

private:
    float _invSampleRate;
    float _attack;
    float _decay;
    float _sustain;
    float _release;
    float _attackIncrement;
    float _decayIncrement;
    float _releaseIncrement;

    enum State {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release,
    } _state = Idle;

    bool _gate = false;
    float _value = 0.f;
};

class Voice {
public:
    Voice(float sampleRate) :
        _osc(sampleRate),
        _filter(sampleRate),
        _envVolume(sampleRate)
    {
        _osc.setWaveform(Oscillator::Square);
    }

    void setGate(bool gate) {
        _envVolume.setGate(gate);
    }

    void setCv(float cv) {
        _osc.setFrequency(BaseFrequency * std::exp2(cv));
    }

    inline float process() {
        return _filter.process(_osc.process()) * _envVolume.process() * _gain;
    }

private:
    static constexpr float BaseFrequency = 440.f / 8.f;

    Oscillator _osc;
    Filter _filter;
    ADSR _envVolume;
    float _gain = 0.3f;
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
            _voice->setCv(_synth._cv);
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
    _handle = _audio.engine().play(*this);
}

Synth::~Synth() {
    _audio.engine().stop(_handle);
}

SoLoud::AudioSourceInstance *Synth::createInstance() {
    return new SynthInstance(*this);
}

void Synth::setGate(bool gate) {
    _gate = gate;
}

void Synth::setCv(float cv) {
    _cv = cv;
}

} // namespace sim
