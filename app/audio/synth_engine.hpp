#pragma once

#include <atomic>
#include <vector>
#include <array>
#include <cmath>

#include "RtMidi.h"

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

class Voice {
public:
    virtual void render() = 0;
    virtual float process() = 0;
    std::atomic<bool> soloist = false;
    std::atomic<bool> mute = true;
    std::atomic<float> gain = 0.5f;

    void noteOn(int note);
    void noteOff();

protected:
    bool editing = true;
    std::atomic<float> frequency = 440.0f;
};

class NOscillator : public Voice {
public:
    NOscillator(int size);
    void render() override;
    float process() override;
private:
    std::vector<Oscillator> oscillators;
};

struct SynthEngine {
    float sampleRate = 48000.0f;
    std::atomic<float> gain = 0.5f;

    NOscillator nOscillator{3};

    void noteOn(int midiNote, float velocity) {
        nOscillator.noteOn(midiNote);
    }

    void noteOff(int midiNote) {
        nOscillator.noteOff();
    }

    void render();
    void refreshMidiMapping();

    std::vector<std::string> midiMapping;
    int currentMidiPort = -1;

    int process(float* out, unsigned int nFrames);

    RtMidiIn* midiIn;
};