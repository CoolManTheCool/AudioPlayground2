#include "audio.hpp"

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

/*

    RtAudio dac;

    if (dac.getDeviceCount() < 1) {
        std::cerr << "No audio devices found.\n";
        return 1;
    }

    AudioState state;

    RtAudio::StreamParameters outParams;
    outParams.deviceId = dac.getDefaultOutputDevice();
    outParams.nChannels = 2;

    unsigned int bufferFrames = 256;   // Try 128, 64, or 32 for lower latency
    unsigned int sampleRate   = 48000;

    try {
        dac.openStream(
            &outParams, nullptr, // output only
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &audioCallback,
            &state
        );

        state.sampleRate = sampleRate;
        dac.startStream();

        std::cout << "Playing for 10 seconds...\n";

        // Change frequency each second (atomic → RT-safe)
        for (int i = 0; i < 10; i++) {
            state.freq.store(440 + i * 50.0, std::memory_order_relaxed);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        dac.stopStream();
    }
    catch (std::exception &e) {
        std::cerr << "An error occurred: " << e.what() << '\n';
        if (dac.isStreamOpen()) dac.closeStream();
    }

    return 0;
    */


/*
int audioCallback(
    void* outputBuffer, void*,
    unsigned int nBufferFrames,
    double, RtAudioStreamStatus status,
    void* userData)
{
    if (status) {
        // Non-RT-safe logging *only for debugging*
        // Remove in actual RT-safe usage.
        std::cerr << "Stream underflow/overflow detected!\n";
    }

    auto *buffer = static_cast<float *>(outputBuffer);
    auto *state  = static_cast<AudioState *>(userData);

    double phase = state->phase;
    double sampleRate = state->sampleRate;

    double freq  = state->freq.load(std::memory_order_relaxed);
    float gain   = state->gain.load(std::memory_order_relaxed);
    double phaseInc = 2.0 * M_PI * freq / sampleRate;

    for (unsigned int i = 0; i < nBufferFrames; i++) {
        float sample = std::sin(phase) * gain;

        // stereo: L = R = sample
        *buffer++ = sample;
        *buffer++ = sample;

        phase += phaseInc;
        if (phase >= 2.0 * M_PI) phase -= 2.0 * M_PI;
    }

    state->phase = phase;
    return 0;
}
*/