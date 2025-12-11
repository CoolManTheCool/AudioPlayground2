#pragma once

#include <atomic>
#include <vector>

#define RELAXED std::memory_order_relaxed

class Oscillator {
public:
    Oscillator(float offset, float gain);
    std::atomic<float> offset;
    std::atomic<float> gain;

    float phase;

    float process(float frequency);

    Oscillator(const Oscillator&) = delete;            // no copying
    Oscillator& operator=(const Oscillator&) = delete;

    // CUSTOM MOVE CONSTRUCTOR
    Oscillator(Oscillator&& other) noexcept
        : offset(other.offset.load(std::memory_order_relaxed)),
          gain(other.gain.load(std::memory_order_relaxed)),
          phase(other.phase) {}
    Oscillator& operator=(Oscillator&&) = default;

    enum Waveform {
        SINE,
        SQUARE,
        SAW,
        TRIANGLE
    };

    void render();
};

class NOscillator {
public:
    NOscillator(int size);
    void render();

    float process(float frequency);

    std::atomic<float> gain = 0.5f;
private:
    std::atomic<bool> mute = true;
    std::atomic<bool> soloist = false;
    bool editing = true;
    std::atomic<float> frequency = 440.0f;
    std::vector<Oscillator> oscillators;
};

struct SynthEngine {
    float sampleRate = 48000.0f;
    std::atomic<float> gain = 0.5f;

    NOscillator nOscillator{3};

    void render();

    int process(float* out, unsigned int nFrames);
};