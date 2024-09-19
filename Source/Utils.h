/*
  ==============================================================================

    Utils.h
    Created: 28 Jan 2024 7:53:00pm
    Author: CS Islay
    Protect yr ears with this one little trick

  ==============================================================================
*/
#pragma once
#include<JuceHeader.h>

inline void protectYourEars(float* buffer, int sampleCount)
{
    if (buffer == nullptr) { return; }
    bool firstWarning = true;
    for (int i = 0; i < sampleCount; ++i) {
        float x = buffer[i];
        bool silence = false;
        if (std::isnan(x)) {
            DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
            silence = true;
        }
        else if (std::isinf(x)) {
            DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            silence = true;
        }
        else if (x < -2.0f || x > 2.0f) { //screaming feedback
            DBG("!!! WARNING: sample out of range, silencing !!!");
            silence = true;
        }
        else if (x < -1.0f) {
            if (firstWarning) {
                DBG("!!! WARNING: sample out range, clamping !!!");
                firstWarning = false;
            }
            buffer[i] = -1.0f;
        }
        else if (x > 1.0f) {
            if (firstWarning) {
                DBG("!!! WARNING: sample out range, clamping !!!");
                firstWarning = false;
            }
            buffer[i] = 1.0f;
        }
        if (silence) {
            memset(buffer, 0, sampleCount * sizeof(float));
            return;
        }
    }
};

// ear protection for single samples
inline float protectYourEarsSample(float sample)
{
    bool silence = false;
    if (std::isnan(sample)) {
        silence = true;
    }
    else if (std::isinf(sample)) {
        silence = true;
    }
    else if (sample < -2.0f || sample > 2.0f) { //screaming feedback
        silence = true;
    }
    else if (sample < -1.0f) {
        sample = -1.0f;
    }
    else if (sample > 1.0f) {
        sample = 1.0f;
    }
    if (silence) {
        sample = 0;
    }
    return sample;
}
