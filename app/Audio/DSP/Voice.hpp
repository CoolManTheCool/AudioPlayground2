#pragma once

#include <memory>
#include <cmath>
#include <atomic>
#include <algorithm>

#include "imgui.h"
#include "imgui-knobs.hpp"

#define RELAXED std::memory_order_relaxed

struct ADSR_Envelope {
    std::atomic<float> attackTime  = 0.015f;  // 15  ms 0-200
    std::atomic<float> decayTime   = 0.18f;   // 180 ms 0-1000
    std::atomic<float> sustainLevel= 0.70f;   // 70  %  0 - 1
    std::atomic<float> releaseTime = 0.25f;   // 250 ms 0 - 1000

    float sampleRate = 48000.0f;

    float targetAmp = 0.0f;
    float currentAmp = 0.0f;

    enum class Stage { Idle, Attack, Decay, Sustain, Release };
    Stage stage = Stage::Idle;

    void noteOn() {
        stage = Stage::Attack;
        targetAmp = 1.0f;
        currentAmp = 0.0f;
    }

    void noteOff() {
        stage = Stage::Release;
        targetAmp = 0.0f;
    }

    void render() {
        float attack = attackTime.load(RELAXED);
        ImGuiKnobs::Knob("Attack", &attack, 0.0f, 1.0f, 0.0f, "%.3f MS", ImGuiKnobVariant_Tick, 35.0f);
        attackTime.store(attack, RELAXED);
        ImGui::SameLine();

        float decay = decayTime.load(RELAXED);
        ImGuiKnobs::Knob("Decay", &decay, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);
        decayTime.store(decay, RELAXED);
        ImGui::SameLine();

        float sustain = sustainLevel.load(RELAXED);
        ImGuiKnobs::Knob("Sustain", &sustain, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);
        sustainLevel.store(sustain, RELAXED);
        ImGui::SameLine();

        float release = releaseTime.load(RELAXED);
        ImGuiKnobs::Knob("Gain", &release, 0.0f, 1.0f, 0.0f, "%.3f %", ImGuiKnobVariant_Tick, 35.0f);
        releaseTime.store(release, RELAXED);
        ImGui::SameLine();
    }

    float update() {
        switch(stage) {
            case Stage::Idle:
                break;

            case Stage::Attack:
                currentAmp += 1.0f / (attackTime * sampleRate);
                if (currentAmp >= 1.0f) {
                    currentAmp = 1.0f;
                    stage = Stage::Decay;
                    targetAmp = sustainLevel;
                }
                break;

            case Stage::Decay:
                currentAmp -= (1.0f - sustainLevel) / (decayTime * sampleRate);
                if (currentAmp <= sustainLevel) {
                    currentAmp = sustainLevel;
                    stage = Stage::Sustain;
                }
                break;

            case Stage::Sustain:
                currentAmp = sustainLevel;
                break;

            case Stage::Release:
                currentAmp -= sustainLevel / (releaseTime * sampleRate);
                if (currentAmp <= 0.0f) {
                    currentAmp = 0.0f;
                    stage = Stage::Idle;
                }
                break;
        }

        return std::clamp(currentAmp, 0.0f, 1.0f);
    }
};

class Voice {
public:
    virtual void startNote(int note, int velocity);
    virtual void stopNote();
    virtual bool isActive() const;
    virtual float process();

protected:
    float velocity = 0.0f;
    float frequency = 0.0f;

    // Simple Envelope (ADSR)
    ADSR_Envelope envelope;
};

float noteNumberToPitch(int note);
float getAmplitude(int velocity);