#include "Audio.hpp"

#include <cmath>

#include "RtMidi.h"
#include "Playground.hpp"

Audio::Audio(Playground* playground) {
    RtAudio::StreamParameters outParams;
    outParams.deviceId = rtAudio.getDefaultOutputDevice();
    outParams.nChannels = 2;

    try {
        rtAudio.openStream(
            &outParams, nullptr, // output only
            RTAUDIO_FLOAT32,
            playground->sampleRate,
            &playground->bufferFrames,
            &Audio::audioCallback,
            playground //user data
        );

        rtAudio.startStream();
    }
    catch (std::exception &e) {
        std::cerr << "An error occurred: " << e.what() << '\n';
        if (rtAudio.isStreamOpen()) rtAudio.closeStream();
    }

    std::cout << "Audio initialized." << std::endl;

    try {
        // Ignore sysex, timing, active sense
        midiIn.ignoreTypes(true, true, true);

        // Give the Playground to the callback
        midiIn.setCallback(&midiCallback, playground);

        refreshMidiMapping();
    } catch (RtMidiError &err) {
        err.printMessage();
    }

    std::cout << "Midi Initialized." << std::endl;
}

Audio::~Audio() {
    if (rtAudio.isStreamOpen()) {
        rtAudio.stopStream();
        rtAudio.closeStream();
    }

    midiIn.closePort();
}

void Audio::render() {
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

                midiIn.closePort();
                midiIn.openPort(i);
                std::cout << "Opened MIDI port: " << midiMapping[i] << std::endl;
            }

            // Highlight the selected item
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::End();
}

int Audio::audioCallback(
    void* outputBuffer, void * /*inputBuffer*/,
    unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus status,
    void* userData
) {
    if (status) {
        // Non-RT-safe logging *only for debugging*
        // Remove in actual RT-safe usage.
        std::cerr << "Stream underflow/overflow detected!\n";
    }

    auto *buffer = static_cast<float *>(outputBuffer);
    auto *playground  = static_cast<Playground *>(userData);

    playground->process(buffer, nBufferFrames);

    return 0;
}

void Audio::midiCallback(
    double /*timeStamp*/,
    std::vector<unsigned char> *message,
    void *userData
) {
    Playground* playground = static_cast<Playground*>(userData);

    if (message->size() < 3) return;

    unsigned char status = message->at(0);
    unsigned char data1  = message->at(1);
    unsigned char data2  = message->at(2);

    unsigned char type = status & 0xF0;

    switch (type) {
        case 0x90: // Note On
            if (data2 != 0)
                playground->noteOn(data1, data2);
            else
                playground->noteOff(data1); // velocity 0 = note off
            break;

        case 0x80: // Note Off
            playground->noteOff(data1);
            break;

        /*
        case 0xB0: // CC
            playground->controlChange(data1, data2);
            break;

        case 0xE0: // Pitch Bend
            playground->pitchBend(data2, data1);
            break;
            */
    }
}

void Audio::refreshMidiMapping() {
    midiMapping.clear();
    
    unsigned int ports = midiIn.getPortCount();
    if (ports == 0) {
        std::cout << "No MIDI ports found!\n";
        return;
    }

    for(unsigned int i = 0; i < ports; ++i) {
        midiMapping.push_back(midiIn.getPortName(i));
    }
}