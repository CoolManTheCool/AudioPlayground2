#pragma once

#include "rtaudio/RtAudio.h"
#include "rtmidi/RtMidi.h"

class Playground;

class Audio {
public:
    Audio(Playground* Playground);
    ~Audio();
    
    void render();

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

    void refreshMidiMapping();
    
private:
    RtAudio rtAudio;
    RtMidiIn midiIn;

    std::vector<std::string> midiMapping;
    int currentMidiPort = -1;
};