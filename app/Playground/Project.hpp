#pragma once

#include <cstdint>
#include <atomic>
#include <vector>
#include <string>
#include <memory>

#include "Instrument.hpp"
#include "DebugBuffer.hpp"

class Transport {
    float bpm = 120.0;
    uint16_t ticksPerBeat = 480;
};

class Note {
    uint32_t start;     // tick (relative to clip)
    uint32_t length;    // in ticks
    uint8_t pitch;     // 0–127
    uint8_t velocity;  // 0–127
};

class Clip {
    uint32_t start;
    uint32_t length;
    std::vector<Note> notes;
};

class Track {
    std::string name;
    std::unique_ptr<Instrument> instrument;
    std::vector<Clip> clips;
};