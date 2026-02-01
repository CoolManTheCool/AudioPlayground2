#include "Playground.hpp"

#include <cmath>
#include <iostream>

#include "imgui.h"
#include "imgui-knobs.hpp"

void Playground::process(float* out, unsigned int nFrames) {

    for (unsigned int i = 0; i < nFrames; i++) {

        float sample = nOscillator.process();

        if(i < debugBuffer.buffer.size()) {
            debugBuffer.buffer[i] = sample;
        }

        out[i * 2 + 0] = sample * 1;//gain.load(RELAXED); // L
        out[i * 2 + 1] = sample * 1;//gain.load(RELAXED); // R
    }
};

void Playground::noteOn(int midiNote, float velocity) {
    nOscillator.startNote(midiNote, static_cast<int>(velocity * 127));
}

void Playground::noteOff(int midiNote) {
    nOscillator.stopNote(midiNote);
}

void Playground::render() {
    ImGui::Begin("Playground");

    //ImGui::Text("Sample Rate: %.1f", sampleRate);

    audio.render();
    nOscillator.render();

    std::vector<float> waveform;
    debugBuffer.getSnapshot(waveform);
    ImGui::PlotLines("Debug Waveform", waveform.data(), static_cast<int>(waveform.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 100));

    if(ImGui::Button("Panic")) {
        nOscillator.panic();
    }

    ImGui::End();
}