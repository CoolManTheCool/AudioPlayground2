#pragma once

#include <atomic>
#include <string>

#define RELAXED std::memory_order_relaxed

/**
 * @brief List of all waveforms
 * @note Visualization of the waveforms available at:
 *       https://www.desmos.com/calculator/9viyik7low
 */
enum class Waveform {
    SINE,
    SQUARE,
    SAW,
    TRIANGLE,
    
    COUNT
};

constexpr int NUM_WAVEFORMS = static_cast<int>(Waveform::COUNT);

std::string waveformToString(Waveform waveform);

class Oscillator {
public:
    Oscillator(Waveform waveform, float gain);
    std::atomic<int> offset{0};
    std::atomic<float> gain;
    std::atomic<size_t> waveform;

    Waveform getWafeform() const;

    float process(float frequency, float& phase);

    Oscillator(const Oscillator&) = delete;            // no copying
    Oscillator& operator=(const Oscillator&) = delete;

    // CUSTOM MOVE CONSTRUCTOR
    Oscillator(Oscillator&& other) noexcept
        : offset(other.offset.load(std::memory_order_relaxed)),
          gain(other.gain.load(std::memory_order_relaxed)) {}
    Oscillator& operator=(Oscillator&&) = default;

    void render();
};