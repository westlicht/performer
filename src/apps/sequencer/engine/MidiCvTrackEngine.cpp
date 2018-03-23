#include "MidiCvTrackEngine.h"

#include "os/os.h"

#include <cmath>


void MidiCvTrackEngine::reset() {
    _pitchBendCv = 0.f;
    _channelPressureCv = 0.f;
    resetVoices();
}

void MidiCvTrackEngine::tick(uint32_t tick) {
}

void MidiCvTrackEngine::receiveMidi(MidiPort port, int channel, const MidiMessage &message) {
    if (port == MidiPort(_midiCvTrack.port()) && (_midiCvTrack.channel() == -1 || _midiCvTrack.channel() == channel)) {
        if (message.isNoteOn()) {
            int note = message.note();
            auto voice = allocateVoice(note, 1);
            voice->ticks = os::ticks();
            voice->note = note;
            voice->pitchCv = noteToCv(note);
            voice->velocityCv = valueToCv(message.velocity());
            voice->pressureCv = 0.f;
            // printVoices();
        } else if (message.isNoteOff()) {
            freeVoice(message.note(), 1);
            // printVoices();
        } else if (message.isKeyPressure()) {
            auto voice = findVoice(0, _voices.size(), message.note());
            voice->pressureCv = valueToCv(message.keyPressure());
        } else if (message.isChannelPressure()) {
            _channelPressureCv = valueToCv(message.channelPressure());
        } else if (message.isPitchBend()) {
            _pitchBendCv = pitchBendToCv(message.pitchBend());
        }
    }
}

float MidiCvTrackEngine::noteToCv(int note) const {
    return (note - 60) * (1.f / 12.f);
}

float MidiCvTrackEngine::valueToCv(int value) const {
    return value * (1.f / 127.f);
}

float MidiCvTrackEngine::pitchBendToCv(int value) const {
    return value * (1.f / 8192.f);
}

void MidiCvTrackEngine::resetVoices() {
    for (auto &voice : _voices) {
        voice.ticks = 0;
    }
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
        DBG("%d %d", voice.ticks, voice.note);
    }
}
