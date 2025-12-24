#include "NOscillator.hpp"

#include "imgui.h"
#include "imgui-knobs.hpp"

#include <stdexcept>

NOscillator::NOscillator(int size) {

    float root = 440.0f;

    oscillators.reserve(size);
    for (int i = 0; i < size; ++i) {
        float freq = root * i / 8;  // harmonic frequency
        float amp = 1.0f / (i + 3); // your amplitude choice

        oscillators.push_back(Oscillator(freq, amp));  // uses move hopefully
    }

    for(int i = 0; i < 30; ++i) {
        voices.push_back(new NOscillatorVoice(oscillators));
    }
}

float NOscillator::process() {
    if(mute.load(RELAXED)) {
        return 0.0f;
    }
    float mixedSample = 0.0f;
    for (auto& voice : voices) {
        mixedSample += voice->process();
    }
    //mixedSample /= static_cast<float>(voices.size()); // Normalize
    return mixedSample * gain.load(RELAXED);
}

void NOscillator::render() {
    ImGui::Text("N-Oscillator");
    ImGui::BeginGroup();
    bool muted = mute.load(RELAXED);
    ImGui::Checkbox("Muted##N-Oscillator", &muted);
    mute.store(muted, RELAXED);

    /*
    bool soloing = soloist.load(RELAXED);
    ImGui::Checkbox("Solo##N-Oscillator", &soloing);
    soloist.store(soloing, RELAXED);
    */
    
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

NOscillatorVoice::NOscillatorVoice(
    std::vector<Oscillator>& oscillators
) : oscillators(oscillators) {
    phases.resize(oscillators.size(), 0.0f);
}

float NOscillatorVoice::process() {
    float envelopeAmp = envelope.update();

    if(envelope.stage == ADSR_Envelope::Stage::Idle) {
        return 0.0f;
    }
    float mixedSample = 0.0f;
    unsigned int index = 0;
    for (auto& osc : oscillators) {
        mixedSample += osc.process(frequency, phases[index]);
        ++index;
    }
    //mixedSample /= static_cast<float>(oscillators.size()); // Normalize
    return mixedSample * envelopeAmp;
}

void NOscillatorVoice::startNote(int note, int velocity) {
    Voice::startNote(note, velocity);
    std::fill(phases.begin(), phases.end(), 0.0f);
}