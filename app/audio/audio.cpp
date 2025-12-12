#include "audio.hpp"

#include "RtMidi.h"

#include <cmath>

void Audio::init(SynthEngine* synthEngine) {
    RtAudio::StreamParameters outParams;
    outParams.deviceId = rtAudio.getDefaultOutputDevice();
    outParams.nChannels = 2;

    unsigned int bufferFrames = 256;   // Try 128, 64, or 32 for lower latency
    unsigned int sampleRate   = 48000;

    try {
        rtAudio.openStream(
            &outParams, nullptr, // output only
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &Audio::audioCallback,
            synthEngine //user data
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

        // Give the synth to the callback
        midiIn.setCallback(&midiCallback, synthEngine);

    } catch (RtMidiError &err) {
        err.printMessage();
    }

    std::cout << "Midi Initialized." << std::endl;

    synthEngine->midiIn = &midiIn;
}

void Audio::shutdown() {
    if (rtAudio.isStreamOpen()) {
        rtAudio.stopStream();
        rtAudio.closeStream();
    }
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
    auto *synthEngine  = static_cast<SynthEngine *>(userData);

    return synthEngine->process(buffer, nBufferFrames);
}

void Audio::midiCallback(
    double timeStamp,
    std::vector<unsigned char> *message,
    void *userData
) {
    SynthEngine* synth = static_cast<SynthEngine*>(userData);

    if (message->size() < 3) return;

    unsigned char status = message->at(0);
    unsigned char data1  = message->at(1);
    unsigned char data2  = message->at(2);

    unsigned char type = status & 0xF0;

    switch (type) {
        case 0x90: // Note On
            if (data2 != 0)
                synth->noteOn(data1, data2);
            else
                synth->noteOff(data1); // velocity 0 = note off
            break;

        case 0x80: // Note Off
            synth->noteOff(data1);
            break;

        /*
        case 0xB0: // CC
            synth->controlChange(data1, data2);
            break;

        case 0xE0: // Pitch Bend
            synth->pitchBend(data2, data1);
            break;
            */
    }
}