#include "Oscillator.hpp"

#include <cmath>

#include "imgui.h"
#include "imgui-knobs.hpp"
#include "Pitch.hpp"

Oscillator::Oscillator(Waveform waveform, float gain) {
    this->gain.store(gain, RELAXED);
    this->waveform.store(static_cast<size_t>(waveform), RELAXED);
}

Waveform Oscillator::getWafeform() const {
    return static_cast<Waveform>(waveform.load(RELAXED));
}

float Oscillator::process(float frequency, float& phase) {
    if(gain.load(RELAXED) == 0.0f) {
        return 0.0f;
    }
    // Simple sine wave oscillator
    float freq = transposeSemitones(frequency, offset.load(RELAXED));
    float g = gain.load(RELAXED);

    float sample;

    switch(getWafeform()) {
    case Waveform::SINE:
        sample = std::sin(2.0f * M_PI * phase) * g;
        break;
    case Waveform::SQUARE:
        sample = (std::fmod(std::floor(2.0 * phase), 2.0) * 2.0) - 1.0;
        break;
    case Waveform::SAW:
        sample = std::fmod(2 * phase, 2) - 1;
        break;
    case Waveform::TRIANGLE:
        sample = 4.0f * std::fabs(phase - 0.5f) - 1.0f;
        break;
    default:
        sample = 0;
    }

    phase += freq / 48000.0f;
    if (phase >= 1.0f) phase -= 1.0f;

    return sample;
}

void Oscillator::render() {
    int offsetVal = offset.load(RELAXED);
    ImGuiKnobs::KnobInt("Offset", &offsetVal, -24, 24, 0.0f, "%i", ImGuiKnobVariant_Tick, 35.0f);
    offset.store(offsetVal, RELAXED);
    ImGui::SameLine();

    float gainValue = gain.load(RELAXED);
    ImGuiKnobs::Knob("Gain", &gainValue, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);
    gain.store(gainValue, RELAXED);
    ImGui::SameLine();

    Waveform wf = getWafeform();
    std::string wfString = waveformToString(wf);


    if (ImGui::BeginCombo("##Waveform", wfString.c_str())) {
        for (int i = 0; i < (int) Waveform::COUNT; i++) {

            bool is_selected = (i == static_cast<int>(wf));

            if (ImGui::Selectable(waveformToString(static_cast<Waveform>(i)).c_str(), is_selected))
                waveform.store(i, RELAXED);

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}

std::string waveformToString(Waveform waveform) {
    switch (waveform) {
    case Waveform::SINE:
        return ("Sine");
    case Waveform::SAW:
        return ("Saw");
    case Waveform::SQUARE:
        return ("Square");
    case Waveform::TRIANGLE:
        return ("Triangle");
    default:
        return ("Undefined");
    }
}