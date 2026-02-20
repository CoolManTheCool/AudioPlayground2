#include "Voice.hpp"

#define EQUAL_TEMPERAMENT true

void Voice::startNote(int note, int velocity) {
    this->frequency = noteNumberToPitch(note);
    this->velocity = getAmplitude(velocity);
    envelope.noteOn(state);
}

void Voice::stopNote() {
    envelope.noteOff(state);
}

bool Voice::isActive() const {
    return state.stage != ADSR_Envelope::Stage::Idle;
}

void Voice::panic() {
    envelope.panic(state);
}

float noteNumberToPitch(int note) {
    #if EQUAL_TEMPERAMENT
    return 440.0f * std::pow(2.0f, (note - 57) / 12.0f);
    // Going down an octave because my midi keyboard is odd like that
    #else
    // Just intonation approximation for demonstration
    static const float justIntonationRatios[12] = {
        1.0f,        // C
        16.0f/15.0f, // C#
        9.0f/8.0f,   // D
        6.0f/5.0f,   // D#
        5.0f/4.0f,   // E
        4.0f/3.0f,   // F
        45.0f/32.0f, // F#
        3.0f/2.0f,   // G
        8.0f/5.0f,   // G#
        5.0f/3.0f,   // A
        9.0f/5.0f,   // A#
        15.0f/8.0f   // B
    };
    int octave = (note / 12) - 5; // MIDI note 60 is C4
    int noteInOctave = note % 12;
    float baseFrequency = 440.0f * std::pow(2.0f, octave);
    return baseFrequency * justIntonationRatios[noteInOctave];
    #endif
}

float getAmplitude(int velocity) {
    return static_cast<float>(velocity) / 127.0f;
}

float Voice::process() {
    return 0.0f;
}