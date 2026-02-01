#pragma once

#include <atomic>
#include <vector>
#include <array>
#include <cmath>

#include "Project.hpp"
#include "Instrument.hpp"
#include "DebugBuffer.hpp"
#include "NOscillator.hpp"
#include "Audio.hpp"

#define RELAXED std::memory_order_relaxed

class Playground {
public:
    unsigned int sampleRate = 48000;
    unsigned int bufferFrames = 256;

    NOscillator nOscillator{3};

    void process(float* out, unsigned int nFrames);
    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);

    void render();

private:
    Audio audio{this};
    DebugBuffer debugBuffer{bufferFrames};
    Transport transport;
    std::vector<Track> tracks;
};