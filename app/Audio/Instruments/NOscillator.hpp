#pragma once

#include "Instrument.hpp"
#include "Oscillator.hpp"
#include "Voice.hpp"

#include <vector>

class NOscillator : public Instrument {
public:
    NOscillator(int size);
    float process() override;
    void render() override;
private:
    std::vector<Oscillator> oscillators;
    //std::atomic<bool> soloist{false};
    std::atomic<bool> mute{false};
    std::atomic<float> gain{0.5f};
};

class NOscillatorVoice : public Voice {
public:
    NOscillatorVoice(std::vector<Oscillator>& oscillators);
    void startNote(int note, int velocity) override;
    float process() override;
private:
    std::vector<Oscillator>& oscillators;
    std::vector<float> phases;
};