#pragma once

#include <atomic>
#include <vector>

#define RELAXED std::memory_order_relaxed

struct DebugBuffer {
    std::vector<float> buffer;
    std::atomic<size_t> writeIndex{0};

    DebugBuffer(size_t size) : buffer(size) {}

    void getSnapshot(std::vector<float>& out) {
        size_t idx = writeIndex.load(RELAXED);
        out.resize(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i)
            out[i] = buffer[(idx + i) % buffer.size()];
    }
};