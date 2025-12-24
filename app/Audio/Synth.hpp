#pragma once

#include <atomic>
#include <vector>
#include <array>
#include <cmath>

#include "NOscillator.hpp"

#include "RtMidi.h"

#define RELAXED std::memory_order_relaxed

struct Synth {
    float sampleRate = 48000.0f;
    std::atomic<float> gain = 0.5f;

    NOscillator nOscillator{3};

    int process(float* out, unsigned int nFrames);

    void noteOn(int midiNote, float velocity);

    void noteOff(int midiNote);

    void render();
    void refreshMidiMapping();

    std::vector<std::string> midiMapping;
    int currentMidiPort = -1;

    RtMidiIn* midiIn;
};