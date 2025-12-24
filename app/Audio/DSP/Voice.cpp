#include "Voice.hpp"

void Voice::startNote(int note, int velocity) {
    active = true;
    this->frequency = noteNumberToPitch(note);
    this->velocity = getAmplitude(velocity);
}

void Voice::stopNote() {
    active = false;
}

bool Voice::isActive() const {
    return active;
}

float noteNumberToPitch(int note) {
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

float getAmplitude(int velocity) {
    return static_cast<float>(velocity) / 127.0f;
}

float Voice::process() {
    return 0.0f;
}