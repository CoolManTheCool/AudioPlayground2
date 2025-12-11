#pragma once

#include "RtAudio.h"
#include "synth_engine.hpp"

class Audio {
public:
    void init(SynthEngine* synthEngine);
    void shutdown();

    static int audioCallback(
        void* outputBuffer, void*,
        unsigned int nBufferFrames,
        double, RtAudioStreamStatus status,
        void* userData
    );
private:
    RtAudio rtAudio;
};