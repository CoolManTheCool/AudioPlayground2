#pragma once

#include <cmath>

float transposeSemitones(float baseHz, int semitoneOffset) {
    return baseHz * std::pow(2.0f, semitoneOffset / 12.0f);
}