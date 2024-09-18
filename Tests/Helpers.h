//
// Created by CS Islay on 18/09/2024.
//

#ifndef HELPERS_H
#define HELPERS_H

#include <cmath>

// Helper function to calculate the period for A440Hz
inline float calculatePeriod (const float noteNumber, const float sampleRate)
{
    const float freq = std::exp2 ((noteNumber - 69.0f) / 12.0f) * 440.0f;
    const float period = sampleRate / freq;
    return period;
}

#endif //HELPERS_H
