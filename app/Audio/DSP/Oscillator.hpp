#pragma once

#include <atomic>

#define RELAXED std::memory_order_relaxed

class Oscillator {
public:
    Oscillator(float offset, float gain);
    std::atomic<float> offset;
    std::atomic<float> gain;

    float process(float frequency, float& phase);

    Oscillator(const Oscillator&) = delete;            // no copying
    Oscillator& operator=(const Oscillator&) = delete;

    // CUSTOM MOVE CONSTRUCTOR
    Oscillator(Oscillator&& other) noexcept
        : offset(other.offset.load(std::memory_order_relaxed)),
          gain(other.gain.load(std::memory_order_relaxed)) {}
    Oscillator& operator=(Oscillator&&) = default;

    enum Waveform {
        SINE,
        SQUARE,
        SAW,
        TRIANGLE
    };

    void render();
};