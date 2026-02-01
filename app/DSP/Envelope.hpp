#pragma once

#include <atomic>
#include <algorithm>

#include "Relaxed.hpp"

struct ADSR_Envelope_State;

struct ADSR_Envelope {
    std::atomic<float> attackTime  = 0.015f;  // 15  ms 0-200
    std::atomic<float> decayTime   = 0.18f;   // 180 ms 0-1000
    std::atomic<float> sustainLevel= 0.70f;   // 70  %  0 - 1
    std::atomic<float> releaseTime = 0.25f;   // 250 ms 0 - 1000

    float sampleRate = 48000.0f;

    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    void  noteOn(ADSR_Envelope_State& state);
    void  noteOff(ADSR_Envelope_State& state);
    void  panic(ADSR_Envelope_State& state);
    float update(ADSR_Envelope_State& state);
    
    void render();

};

struct ADSR_Envelope_State {
    ADSR_Envelope::Stage stage = ADSR_Envelope::Stage::Idle;

    float targetAmp = 0.0f;
    float currentAmp = 0.0f;
};