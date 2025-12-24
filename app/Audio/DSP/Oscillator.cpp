#pragma once

#include "Oscillator.hpp"

#include <cmath>

#include "imgui.h"
#include "imgui-knobs.hpp"

Oscillator::Oscillator(float offset, float gain) {
    this->offset.store(offset, RELAXED);
    this->gain.store(gain, RELAXED);
}

float Oscillator::process(float frequency, float& phase) {
    if(gain.load(RELAXED) == 0.0f) {
        return 0.0f;
    }
    // Simple sine wave oscillator
    float freq = offset.load(RELAXED) + frequency;
    float g = gain.load(RELAXED);

    float sample = std::sin(2.0f * M_PI * phase) * g;

    phase += freq / 48000.0f;
    if (phase >= 1.0f) phase -= 1.0f;

    return sample;
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