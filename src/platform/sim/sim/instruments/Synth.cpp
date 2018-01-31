#include "Synth.h"

namespace sim {

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

class ADSR {
public:
    ADSR(float sampleRate) :
        _invSampleRate(1.f / sampleRate)
    {
        setAttack(0.01f);
        setDecay(0.f);
        setSustain(1.f);
        setRelease(0.5f);
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
        // return _gate ? 1.f : 0.f;
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
    float _attack = 0.01f;
    float _decay = 0.f;
    float _sustain = 1.f;
    float _release = 0.5f;
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
        _envVolume(sampleRate)
    {
        _osc.setWaveform(Oscillator::Triangle);
    }

    void setGate(bool gate) {
        _envVolume.setGate(gate);
    }

    void setCV(float cv) {
        _osc.setFrequency(BaseFrequency * std::exp2(cv));
    }

    inline float process() {
        return _osc.process() * _envVolume.process();
    }

private:
    static constexpr float BaseFrequency = 440.f / 32.f;

    Oscillator _osc;
    ADSR _envVolume;
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
