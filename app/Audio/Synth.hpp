#pragma once

#include <atomic>
#include <vector>
#include <array>
#include <cmath>

#include "NOscillator.hpp"

#include "rtmidi/RtMidi.h"

#define RELAXED std::memory_order_relaxed

struct DebugBuffer {
    std::vector<float> buffer;
    std::atomic<size_t> writeIndex{0};

    DebugBuffer(size_t size) : buffer(size) {}

    void push(float value) {
        buffer[writeIndex++ % buffer.size()] = value;
    }

    void getSnapshot(std::vector<float>& out) {
        size_t idx = writeIndex.load();
        out.resize(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i)
            out[i] = buffer[(idx + i) % buffer.size()];
    }
};

struct Synth {
    float sampleRate = 48000.0f;
    std::atomic<float> gain = 0.5f;

    NOscillator nOscillator{4};
    DebugBuffer debugBuffer{48000/256};

    void process(float* out, unsigned int nFrames);

    void noteOn(int midiNote, float velocity);

    void noteOff(int midiNote);

    void render();
    void refreshMidiMapping();

    std::vector<std::string> midiMapping;
    int currentMidiPort = -1;

    RtMidiIn* midiIn;
};