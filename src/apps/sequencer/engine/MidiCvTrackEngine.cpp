#include "MidiCvTrackEngine.h"
#include "Engine.h"
#include "MidiUtils.h"

#include "os/os.h"

#include <cmath>
#include <cinttypes>


void MidiCvTrackEngine::reset() {
    _arpeggiatorEnabled = false;
    _activity = false;
    _pitchBendCv = 0.f;
    _channelPressureCv = 0.f;
    resetVoices();
}

void MidiCvTrackEngine::tick(uint32_t tick) {
    if (_arpeggiatorEnabled) {
        tickArpeggiator(tick);
    }
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
}

bool MidiCvTrackEngine::receiveMidi(MidiPort port, const MidiMessage &message) {
    bool consumed = false;

    if (_arpeggiatorEnabled) {
        if (MidiUtils::matchSource(port, message, _midiCvTrack.source())) {
            if (message.isNoteOn()) {
                _arpeggiatorEngine.noteOn(message.note());
            } else if (message.isNoteOff()) {
                _arpeggiatorEngine.noteOff(message.note());
            }

            consumed = true;
        }
    } else {
        if (MidiUtils::matchSource(port, message, _midiCvTrack.source())) {
            if (message.isNoteOn()) {
                addVoice(message.note(), message.velocity());
                // printVoices();
            } else if (message.isNoteOff()) {
                removeVoice(message.note());
                // printVoices();
            } else if (message.isKeyPressure()) {
                auto voice = findVoice(0, _voices.size(), message.note());
                if (voice) {
                    voice->pressureCv = valueToCv(message.keyPressure());
                }
            } else if (message.isChannelPressure()) {
                _channelPressureCv = valueToCv(message.channelPressure());
            } else if (message.isPitchBend()) {
                _pitchBendCv = pitchBendToCv(message.pitchBend());
            }

            updateActivity();

            consumed = true;
        }
    }


    return consumed;
}

bool MidiCvTrackEngine::activity() const {
    return _activity;
}

bool MidiCvTrackEngine::gateOutput(int index) const {
    auto ticks = _voices[index % _midiCvTrack.voices()].ticks;
    if (_midiCvTrack.retrigger()) {
        return ticks > 0 && ticks - os::ticks() > RetriggerDelay;
    } else {
        return ticks > 0;
    }
}

float MidiCvTrackEngine::cvOutput(int index) const {
    int voices = _midiCvTrack.voices();
    int signals = int(_midiCvTrack.voiceConfig()) + 1;
    int totalOutputs = voices * signals;
    index %= totalOutputs;
    int voiceIndex = index % voices;
    int signalIndex = index / voices;
    const auto &voice = _voices[voiceIndex];
    switch (signalIndex) {
    case 0: return voice.pitchCv + _pitchBendCv;
    case 1: return voice.velocityCv;
    case 2: return voice.pressureCv + _channelPressureCv;
    }
    return 0.f;
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
    _arpeggiatorEngine.tick(tick);

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
    const auto range = Types::voltageRangeInfo(_midiCvTrack.modulationRange());
    return value * (1.f / 127.f) * (range->hi - range->lo) + range->lo;
}

float MidiCvTrackEngine::pitchBendToCv(int value) const {
    return value * _midiCvTrack.pitchBendRange() * (1.f / (12 * 8192));
}

void MidiCvTrackEngine::resetVoices() {
    for (auto &voice : _voices) {
        voice.ticks = 0;
    }
}

void MidiCvTrackEngine::addVoice(int note, int velocity) {
    auto voice = allocateVoice(note, _midiCvTrack.voices());
    voice->ticks = os::ticks();
    voice->note = note;
    voice->pitchCv = noteToCv(note);
    voice->velocityCv = valueToCv(velocity);
    voice->pressureCv = 0.f;
}

void MidiCvTrackEngine::removeVoice(int note) {
    freeVoice(note, _midiCvTrack.voices());
}

MidiCvTrackEngine::Voice *MidiCvTrackEngine::allocateVoice(int note, int numVoices) {
    auto lruActive = lruVoice(0, numVoices);
    auto lruInactive = lruVoice(numVoices, _voices.size());
    if (lruInactive) {
        std::swap(*lruActive, *lruInactive);
    }
    return lruActive;
}

void MidiCvTrackEngine::freeVoice(int note, int numVoices) {
    Voice *active = nullptr;
    for (int i = 0; i < int(_voices.size()); ++i) {
        auto voice = &_voices[i];
        if (voice->ticks > 0 && voice->note == note) {
            voice->ticks = 0;
            if (!active && i < numVoices) {
                active = voice;
            }
        }
    }

    if (active) {
        auto mruInactive = mruVoice(numVoices, _voices.size());
        if (mruInactive) {
            std::swap(*active, *mruInactive);
        }
    }
}

MidiCvTrackEngine::Voice *MidiCvTrackEngine::findVoice(int begin, int end, int note) {
    for (int i = begin; i < end; ++i) {
        auto voice = &_voices[i];
        if (voice->ticks > 0 && voice->note == note) {
            return voice;
        }
    }
    return nullptr;
}

MidiCvTrackEngine::Voice *MidiCvTrackEngine::lruVoice(int begin, int end) {
    Voice *lru = nullptr;
    for (int i = begin; i < end; ++i) {
        auto voice = &_voices[i];
        if (!lru || voice->ticks < lru->ticks) {
            lru = voice;
        }
    }
    return lru;
}

MidiCvTrackEngine::Voice *MidiCvTrackEngine::mruVoice(int begin, int end) {
    Voice *mru = nullptr;
    for (int i = begin; i < end; ++i) {
        auto voice = &_voices[i];
        if (voice->ticks > 0 && (!mru || voice->ticks > mru->ticks)) {
            mru = voice;
        }
    }
    return mru;
}

void MidiCvTrackEngine::printVoices() {
    DBG("voices");
    for (auto voice : _voices) {
        DBG("%" PRIu32 " %" PRIu8, voice.ticks, voice.note);
    }
}

void MidiCvTrackEngine::updateActivity() {
    _activity = false;
    for (int i = 0; i < _midiCvTrack.voices(); ++i) {
        if (_voices[i].ticks > 0) {
            _activity = true;
            break;
        }
    }
}
