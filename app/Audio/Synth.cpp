#include "Synth.hpp"

#include <cmath>
#include <iostream>

#include "imgui.h"
#include "imgui-knobs.hpp"

int Synth::process(float* out, unsigned int nFrames) {

    for (unsigned int i = 0; i < nFrames; i++) {

        float sample = nOscillator.process();

        out[i * 2 + 0] = sample * gain.load(RELAXED); // L
        out[i * 2 + 1] = sample * gain.load(RELAXED); // R
    }

    return 0;
};

void Synth::noteOn(int midiNote, float velocity) {
    nOscillator.startNote(midiNote, static_cast<int>(velocity * 127));
}

void Synth::noteOff(int midiNote) {
    nOscillator.stopNote(midiNote);
}

void Synth::render() {
    ImGui::Begin("Synth Engine");
    if (ImGui::Button("Refresh MIDI Mapping")) {
        refreshMidiMapping();
    }
    // Display the current port name or a placeholder
    const char* currentLabel = 
        (currentMidiPort >= 0 && currentMidiPort < (int) midiMapping.size())
            ? midiMapping[currentMidiPort].c_str()
            : "<none>";

    if (ImGui::BeginCombo("MIDI Input Port", currentLabel)) {
        for (int i = 0; i < (int) midiMapping.size(); i++) {

            bool is_selected = (i == currentMidiPort);

            if (ImGui::Selectable(midiMapping[i].c_str(), is_selected)) {
                currentMidiPort = i;

                midiIn->closePort();
                midiIn->openPort(i);
                std::cout << "Opened MIDI port: " << midiMapping[i] << std::endl;
            }

            // Highlight the selected item
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::Text("Sample Rate: %.1f", sampleRate);
    float gainValue = gain.load();
    ImGui::SliderFloat("Gain", &gainValue, 0.0f, 1.0f);
    gain.store(gainValue, RELAXED);

    ImGui::BeginChild("Voices");
    nOscillator.render();
    ImGui::Separator();

    ImGui::EndChild();

    ImGui::End();
}

void Synth::refreshMidiMapping() {
    midiMapping.clear();
    
    unsigned int ports = midiIn->getPortCount();
    if (ports == 0) {
        std::cout << "No MIDI ports found!\n";
        return;
    }

    for(unsigned int i = 0; i < ports; ++i) {
        midiMapping.push_back(midiIn->getPortName(i));
    }
}