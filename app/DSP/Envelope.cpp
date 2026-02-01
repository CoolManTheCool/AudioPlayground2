#include "Envelope.hpp"

void ADSR_Envelope::noteOn(ADSR_Envelope_State& state)  {
    state.stage = Stage::Attack;
    state.targetAmp = 1.0f;
    state.currentAmp = 0.0f;
}

void ADSR_Envelope::noteOff(ADSR_Envelope_State& state) {
    state.stage = Stage::Release;
    state.targetAmp = 0.0f;
}

void ADSR_Envelope::render() {
    float attack = attackTime.load(RELAXED);
    float decay = decayTime.load(RELAXED);
    float sustain = sustainLevel.load(RELAXED);
    float release = releaseTime.load(RELAXED);

    ImGuiKnobs::Knob("Attack", &attack, 0.0f, 1.0f, 0.0f, "%.3f MS", ImGuiKnobVariant_Tick, 35.0f);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Decay", &decay, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Sustain", &sustain, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Release", &release, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);

    attackTime.store(attack, RELAXED);
    decayTime.store(decay, RELAXED);
    sustainLevel.store(sustain, RELAXED);
    releaseTime.store(release, RELAXED);
}

void ADSR_Envelope::panic(ADSR_Envelope_State& state) {
    state.stage = Stage::Idle;
    state.currentAmp = 0.0f;
    state.targetAmp = 0.0f;
}

float ADSR_Envelope::update(ADSR_Envelope_State& state)  {
    switch(state.stage) {
    case Stage::Idle:
        break;

    case Stage::Attack:
        state.currentAmp += 1.0f / (attackTime * sampleRate);
        if (state.currentAmp >= 1.0f) {
            state.currentAmp = 1.0f;
            state.stage = Stage::Decay;
            state.targetAmp = sustainLevel;
        }
        break;

    case Stage::Decay:
        state.currentAmp -= (1.0f - state.targetAmp) / (decayTime * sampleRate);
        if (state.currentAmp <= state.targetAmp) {
            state.currentAmp = state.targetAmp;
            state.stage = Stage::Sustain;
        }
        break;

    case Stage::Sustain:
        state.currentAmp = state.targetAmp;
        break;

    case Stage::Release:
        state.currentAmp -= state.currentAmp / (releaseTime * sampleRate);
        if (state.currentAmp <= 0.0001f) {
            state.currentAmp = 0.0f;
            state.stage = Stage::Idle;
        }
        break;
    }

    return std::clamp(state.currentAmp, 0.0f, 1.0f);
}