#pragma once

#include <memory>
#include <cmath>

class Voice {
public:
    virtual void startNote(int note, int velocity);
    virtual void stopNote();
    virtual bool isActive() const;
    virtual float process();

protected:
    bool active = false;
    float velocity = 0.0f;
    float frequency = 0.0f;
};

float noteNumberToPitch(int note);
float getAmplitude(int velocity);