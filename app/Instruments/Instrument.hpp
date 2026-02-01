#pragma once

#include <atomic>
#include <vector>
#include <map>
#include "Voice.hpp"

class Instrument {
public:
    Instrument() = default;
    virtual void render() = 0;
    virtual float process() = 0;

    void panic();

    void startNote(int note, int velocity);
    void stopNote(int note);

protected:
    ADSR_Envelope envelope;

    bool editing = true;
    std::vector<Voice*> voices;
    std::map<int, Voice*> activeNotes;
    int nextVoiceIndex = 0;
};