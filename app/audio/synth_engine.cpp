#include "synth_engine.hpp"

#include <cmath>

#include "imgui.h"
#include "imgui-knobs.hpp"

Oscillator::Oscillator(float offset, float gain) {
    this->offset.store(offset, RELAXED);
    this->gain.store(gain, RELAXED);
    this->phase = 0.0f;
}

float Oscillator::process(float frequency) {
    // Simple sine wave oscillator
    float freq = offset.load(RELAXED) + frequency;
    float g = gain.load(RELAXED);

    float sample = std::sin(2.0f * M_PI * phase) * g;

    phase += freq / 48000.0f;
    if (phase >= 1.0f) phase -= 1.0f;

    return sample;
}

float NOscillator::process(float frequency) {
    if(mute.load(RELAXED)) {
        return 0.0f;
    }
    float mixedSample = 0.0f;
    for (auto& osc : oscillators) {
        mixedSample += osc.process(frequency);
    }
    //mixedSample /= static_cast<float>(oscillators.size()); // Normalize
    return mixedSample * gain.load(RELAXED);
}

int SynthEngine::process(float* out, unsigned int nFrames) {

    for (unsigned int i = 0; i < nFrames; i++) {


        float sample = nOscillator.process(440.0f);

        out[i * 2 + 0] = sample * gain.load(RELAXED); // L
        out[i * 2 + 1] = sample * gain.load(RELAXED); // R
    }

    return 0;
};

NOscillator::NOscillator(int size) {
    float root = 440.0f;

    oscillators.reserve(size);
    for (int i = 0; i < size; ++i) {
        float harmonic = i;                 // 1, 2, 3, ...
        float freq = root * harmonic;             // harmonic frequency
        float amp = 1.0f / (i + 3);                  // your amplitude choice

        oscillators.push_back(Oscillator(freq, amp));  // uses move hopefully
    }
}

void SynthEngine::render() {
    ImGui::Begin("Synth Engine");
    ImGui::Text("Sample Rate: %.1f", sampleRate);
    float gainValue = gain.load();
    ImGui::SliderFloat("Gain", &gainValue, 0.0f, 1.0f);
    gain.store(gainValue, RELAXED);

    ImGui::BeginChild("Voices");
    nOscillator.render();
    ImGui::Separator();

    ImGui::EndChild();

    ImGui::End();
}

void NOscillator::render() {
    ImGui::Text("N-Oscillator");
    ImGui::BeginGroup();
    bool muted = mute.load(RELAXED);
    ImGui::Checkbox("Muted##N-Oscillator", &muted);
    mute.store(muted, RELAXED);

    bool soloing = soloist.load(RELAXED);
    ImGui::Checkbox("Solo##N-Oscillator", &soloing);
    soloist.store(soloing, RELAXED);
    
    ImGui::Checkbox("Editing##N-Oscillator", &editing);
    ImGui::EndGroup();

    ImGui::SameLine();

    float gainValue = gain.load();
    ImGuiKnobs::Knob("Gain", &gainValue, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 40.0f);
    gain.store(gainValue, RELAXED);

    if (editing) {
        ImGui::Begin("NOscillator Editor");
        for(size_t i = 0; i < oscillators.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            ImGui::Text("Oscillator %zu", i);
            oscillators[i].render();
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::End();
    }
    
}

void Oscillator::render() {
    float offsetVal = offset.load(RELAXED);
    ImGuiKnobs::Knob("Offset", &offsetVal, 0.0f, 100.0f, 0.0f, "%.1f Hz", ImGuiKnobVariant_Tick, 40.0f);
    offset.store(offsetVal, RELAXED);
    ImGui::SameLine();

    float gainValue = gain.load(RELAXED);
    ImGuiKnobs::Knob("Gain", &gainValue, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 40.0f);
    gain.store(gainValue, RELAXED);
}