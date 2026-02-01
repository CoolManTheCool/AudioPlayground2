#pragma once

#include <memory>
#include <cmath>
#include <atomic>
#include <algorithm>

#include "Envelope.hpp"

class Voice {
public:
    Voice(ADSR_Envelope& env) : envelope(env) {}
    virtual void startNote(int note, int velocity);
    virtual void stopNote();
    virtual bool isActive() const;
    virtual float process();
    void panic();

protected:
    float velocity = 0.0f;
    float frequency = 0.0f;

    ADSR_Envelope& envelope;

    ADSR_Envelope_State state;
};

float noteNumberToPitch(int note);
float getAmplitude(int velocity);