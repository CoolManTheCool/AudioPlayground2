#include "Instrument.hpp"

#include <iostream>

void Instrument::panic() {
    for (auto& voice : voices) {
        voice->panic();
    }
    activeNotes.clear();
}

void Instrument::startNote(int note, int velocity) {
    if (voices.empty()) return;

    // Pick the next voice in round-robin
    Voice* voice = voices[nextVoiceIndex];
    voice->startNote(note, velocity);

    // Track which voice is playing this note
    activeNotes[note] = voice;

    // Move to the next voice, wrap around
    nextVoiceIndex = (nextVoiceIndex + 1) % voices.size();
}

void Instrument::stopNote(int note) {
    auto it = activeNotes.find(note);
    if (it != activeNotes.end()) {
        it->second->stopNote();
        //activeNotes.erase(it);
    }
}
