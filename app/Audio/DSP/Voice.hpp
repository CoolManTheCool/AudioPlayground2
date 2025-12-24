#pragma once

#include <memory>
#include <cmath>
#include <atomic>
#include <algorithm>

struct ADSR_Envelope {
    std::atomic<float> attackTime = 0.01f;
    std::atomic<float> decayTime = 0.5f;
    std::atomic<float> sustainLevel = 0.5f;
    std::atomic<float> releaseTime = 0.5f;

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