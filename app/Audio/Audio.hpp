#pragma once

#include "rtaudio/RtAudio.h"
#include "Synth.hpp"

class Audio {
public:
    void init(Synth* synth);
    void shutdown();

    static int audioCallback(
        void* outputBuffer, void*,
        unsigned int nBufferFrames,
        double, RtAudioStreamStatus status,
        void* userData
    );

    static void midiCallback(
        double timeStamp,
        std::vector<unsigned char> *message,
        void *userData
    );
private:
    RtAudio rtAudio;
    RtMidiIn midiIn;
};