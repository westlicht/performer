#include "MidiCvTrackEngine.h"
#include "Engine.h"
#include "Slide.h"
#include "MidiUtils.h"

#include "os/os.h"

#include <cmath>
#include <cinttypes>


void MidiCvTrackEngine::reset() {
    _arpeggiatorEnabled = false;
    _activity = false;
    _pitchBend = 0;
    _channelPressure = 0;
    _slideActive = false;
    resetVoices();
}

void MidiCvTrackEngine::restart() {
}

TrackEngine::TickResult MidiCvTrackEngine::tick(uint32_t tick) {
    if (_arpeggiatorEnabled) {
        tickArpeggiator(tick);
    }

    return TickResult::NoUpdate;
}

void MidiCvTrackEngine::update(float dt) {
    updateArpeggiator();

    // run arpeggiator even if clock is not running
    if (_arpeggiatorEnabled && !_engine.clockRunning()) {
        _arpeggiatorTime += dt;
        float tickDuration = _engine.clock().tickDuration();
        while (_arpeggiatorTime > tickDuration) {
            tickArpeggiator(_arpeggiatorTick);
            _arpeggiatorTime -= tickDuration;
            ++_arpeggiatorTick;
        }
    }

    // update monophonic portamento
    if (_midiCvTrack.voices() == 1) {
        _pitchCvOutputTarget = noteToCv(_voices.front().note + _midiCvTrack.transpose()) + pitchBendToCv(_pitchBend);
        if (_slideActive && _midiCvTrack.slideTime() > 0) {
            _pitchCvOutput = Slide::applySlide(_pitchCvOutput, _pitchCvOutputTarget, _midiCvTrack.slideTime(), dt);
        } else {
            _pitchCvOutput = _pitchCvOutputTarget;
        }
    }
}

bool MidiCvTrackEngine::receiveMidi(MidiPort port, const MidiMessage &message) {
    // filter notes that are not in the key range
    if (message.isNoteOn() || message.isNoteOff()) {
        if (message.note() < _midiCvTrack.lowNote() || message.note() > _midiCvTrack.highNote()) {
            return false;
        }
    }

    bool consumed = false;

    if (MidiUtils::matchSource(port, message, _midiCvTrack.source())) {
        if (_arpeggiatorEnabled) {
            if (message.isNoteOn()) {
                _arpeggiatorEngine.noteOn(message.note());
                consumed = true;
            } else if (message.isNoteOff()) {
                _arpeggiatorEngine.noteOff(message.note());
                consumed = true;
            }
        } else {
            if (message.isNoteOn()) {
                addVoice(message.note(), message.velocity());
                consumed = true;
            } else if (message.isNoteOff()) {
                removeVoice(message.note());
                consumed = true;
            } else if (message.isKeyPressure()) {
                auto voice = findVoice(message.note());
                if (voice) {
                    voice->pressure = message.keyPressure();
                }
                consumed = true;
            } else if (message.isChannelPressure()) {
                _channelPressure = message.channelPressure();
                consumed = true;
            } else if (message.isPitchBend()) {
                _pitchBend = message.pitchBend();
                consumed = true;
            }

            updateActivity();
        }
    }

    return consumed;
}

bool MidiCvTrackEngine::activity() const {
    return _activity;
}

bool MidiCvTrackEngine::gateOutput(int index) const {
    int voiceIndex = _voiceByOutput[index % _midiCvTrack.voices()];
    if (voiceIndex != -1) {
        const auto &voice = _voices[voiceIndex];
        uint32_t delay = _midiCvTrack.retrigger() ? RetriggerDelay : 0;
        return !mute() && voice.isActive() && (voice.ticks - os::ticks()) >= delay;
    }
    return false;
}

float MidiCvTrackEngine::cvOutput(int index) const {
    int transpose = _midiCvTrack.transpose();
    int voices = _midiCvTrack.voices();
    int signalCount = _midiCvTrack.voiceSignalCount();
    int totalOutputs = voices * signalCount;
    index %= totalOutputs;
    int voiceIndex = index % voices;
    int signalIndex = index / voices;

    voiceIndex = _voiceByOutput[index % _midiCvTrack.voices()];
    if (voiceIndex != -1) {
        const auto &voice = _voices[voiceIndex];
        switch (_midiCvTrack.voiceSignalByIndex(signalIndex)) {
        case MidiCvTrack::VoiceSignal::Pitch:
            return voices == 1 ? _pitchCvOutput : noteToCv(voice.note + transpose) + pitchBendToCv(_pitchBend);
        case MidiCvTrack::VoiceSignal::Velocity:
            return valueToCv(voice.velocity);
        case MidiCvTrack::VoiceSignal::Pressure:
            return valueToCv(voice.pressure) + valueToCv(_channelPressure);
        }
    }
    return 0.f;
}

void MidiCvTrackEngine::updateActivity() {
    _activity = std::any_of(_voices.begin(), _voices.end(), [] (const Voice &voice) {
        return voice.isActive();
    });
}

void MidiCvTrackEngine::updateArpeggiator() {
    const auto &arpeggiator = _midiCvTrack.arpeggiator();
    if (arpeggiator.enabled() && !_arpeggiatorEnabled) {
        // enable arpeggiator
        resetVoices();
        _arpeggiatorEngine.reset();
        _arpeggiatorEnabled = true;
        _arpeggiatorTime = 0.f;
        _arpeggiatorTick = 0;
    } else if (!arpeggiator.enabled() && _arpeggiatorEnabled) {
        // disable arpeggiator
        resetVoices();
        _arpeggiatorEnabled = false;
    }
}

void MidiCvTrackEngine::tickArpeggiator(uint32_t tick) {
    _arpeggiatorEngine.tick(tick, swing());

    ArpeggiatorEngine::Event event;
    while (_arpeggiatorEngine.getEvent(tick, event)) {
        if (event.action == ArpeggiatorEngine::Event::NoteOn) {
            addVoice(event.note, event.velocity);
        } else if (event.action == ArpeggiatorEngine::Event::NoteOff) {
            removeVoice(event.note);
        }

        updateActivity();
    }
}

float MidiCvTrackEngine::noteToCv(int note) const {
    return (note - 60) * (1.f / 12.f);
}

float MidiCvTrackEngine::valueToCv(int value) const {
    const auto &range = Types::voltageRangeInfo(_midiCvTrack.modulationRange());
    return range.denormalize(value * (1.f / 127.f));
}

float MidiCvTrackEngine::pitchBendToCv(int value) const {
    return value * _midiCvTrack.pitchBendRange() * (1.f / (12 * 8192));
}

void MidiCvTrackEngine::resetVoices() {
    for (auto &voice : _voices) {
        voice.reset();
    }
    _voiceByOutput.fill(-1);
    _nextOutput = -1;
}

void MidiCvTrackEngine::addVoice(int note, int velocity) {
    // activate slide if there already are active voices
    _slideActive = _midiCvTrack.voices() == 1 && std::any_of(_voices.begin(), _voices.end(), [] (const Voice &voice) {
        return voice.isActive();
    });

    // find a free voice
    const auto it = std::find_if(_voices.begin(), _voices.end(), [] (const Voice &voice) {
        return !voice.isActive() && !voice.isAllocated();
    });

    // override last voice (lowest priority) if all are used
    auto &voice = it == _voices.end() ? _voices.back() : *it;

    voice.reset();
    voice.ticks = os::ticks();
    voice.note = note;
    voice.velocity = velocity;
    voice.pressure = 0;

    sortVoices();
}

void MidiCvTrackEngine::removeVoice(int note) {
    auto voice = findVoice(note);
    if (voice) {
        voice->release();
    }

    sortVoices();
}

MidiCvTrackEngine::Voice *MidiCvTrackEngine::findVoice(int note) {
    auto it = std::find_if(_voices.begin(), _voices.end(), [note] (const Voice &voice) {
        return voice.isActive() && voice.note == note;
    });

    return it == _voices.end() ? nullptr : it;
}

void MidiCvTrackEngine::sortVoices() {
    // move all active voices to front of array
    auto activeEnd = std::stable_partition(_voices.begin(), _voices.end(), [] (const Voice &voice) {
        return voice.isActive();
    });

    // sort based on note priority
    auto notePriority = _midiCvTrack.notePriority();
    std::sort(_voices.begin(), activeEnd, [notePriority] (const Voice &a, const Voice &b) {
        switch (notePriority) {
        case MidiCvTrack::NotePriority::LastNote:
            return a.ticks > b.ticks;
        case MidiCvTrack::NotePriority::FirstNote:
            return a.ticks < b.ticks;
        case MidiCvTrack::NotePriority::LowestNote:
            return a.note < b.note;
        case MidiCvTrack::NotePriority::HighestNote:
            return a.note > b.note;
        case MidiCvTrack::NotePriority::Last:
            break;
        }
        return false;
    });

    // update voice allocation
    int voices = _midiCvTrack.voices();

    // reset excess voices
    for (int i = 0; i < int(_voices.size()); ++i) {
        auto &voice = _voices[i];
        if (voice.output >= voices) {
            voice.reset();
        }
    }

    // helper to allocate a new voice output
    auto allocateOutput = [this, voices] () -> int {
        // try to allocate output in round-fashion, either a new output or released voice
        for (int i = 0; i < int(VoiceCount); ++i) {
            ++_nextOutput;
            _nextOutput = _nextOutput >= voices ? 0 : _nextOutput;
            bool isFree = std::none_of(_voices.begin(), _voices.end(), [this] (const Voice &v) {
                return v.isActive() && v.output == _nextOutput;
            });
            if (isFree) {
                for (auto &voice : _voices) {
                    if (voice.output == _nextOutput) {
                        voice.output = -1;
                    }
                }
                return _nextOutput;
            }
        }

        // otherwise steal lowest priority output
        for (int i = int(_voices.size()) - 1; i >= 0; --i) {
            auto &voice = _voices[i];
            if (voice.isAllocated()) {
                int output = voice.output;
                voice.output = -1;
                return output;
            }
        }

        return -1;
    };

    // allocate new voices in round-robin fashion
    for (int i = 0; i < voices; ++i) {
        auto &voice = _voices[i];
        if (voice.isActive() && !voice.isAllocated()) {
            voice.output = allocateOutput();
        }
    }

    // update allocation map
    _voiceByOutput.fill(-1);
    for (int i = 0; i < int(_voices.size()); ++i) {
        const auto &voice = _voices[i];
        if (voice.isAllocated()) {
            _voiceByOutput[voice.output] = i;
        }
    }

    // printVoices();
}

void MidiCvTrackEngine::printVoices() {
    DBG("voices");
    for (auto voice : _voices) {
        DBG("%" PRIu32 " %d %d", voice.ticks, int(voice.note), int(voice.output));
    }
}
