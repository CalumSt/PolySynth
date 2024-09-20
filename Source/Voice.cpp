#pragma once
#include "Voice.h"

float Voice::render(float input)
{
    // get the oscillator samples
    // subtract and add noise input
    // apply envelope
    // TODO: replace samples and oscillators with arrays of nextSamples and Oscillators
    auto nextSample = oscillator.render();
    auto nextSample2 = oscillator2.render();
    float outputSample = nextSample + nextSample2 + input;
    outputSample = filter.render(outputSample);

    float envelopeSample = env.nextValue();
    return outputSample * envelopeSample;
}

void Voice::reset()
{
    note = 0;
    velocity = 0;
    env.reset();
    oscillator.reset();
    oscillator2.reset();
    filter.reset();

    panLeft = 0.707f;
    panRight = 0.707f;
}

void Voice::noteOff()
{
    env.release();
}

void Voice::update()
{
    // update panning
    float panning = std::clamp((static_cast<float>(note) - 60.0f) / 24.0f, -1.0f, 1.0f); // notes outside this range are clamped
    panLeft = std::sin(PI_OVER_FOUR * (1.0f - panning));
    panRight = std::sin(PI_OVER_FOUR * (1.0f + panning));
}

void Voice::setSampleRate (const float sampleRate)
{
    env.setSampleRate(sampleRate);
    oscillator.sampleRate = sampleRate;
    oscillator2.sampleRate = sampleRate;
}
