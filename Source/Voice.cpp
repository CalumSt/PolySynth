#pragma once
#include "Voice.h"

void Voice::noteOn(int note, int velocity)
{
    // Do Nothing
}

float Voice::render(float input)
{
    // get the oscillator samples
    // subtract and add noise input
    // apply envelope
    // TODO: replace samples and oscillators with arrays of nextSamples and Oscillators
    auto nextSample = oscillator.render();
    auto nextSample2 = oscillator2.render();

    float envelopeSample = env.nextValue();
    float outputSample = nextSample + nextSample2 + input;
    return outputSample * envelopeSample;
}

void Voice::reset()
{
    note = 0;
    velocity = 0;
    env.reset();
    oscillator.reset();
    oscillator2.reset();

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
    float panning = std::clamp((note - 60.0f) / 24.0f, -1.0f, 1.0f); // notes outside this range are clamped
    panLeft = std::sin(PI_OVER_FOUR * (1.0f - panning));
    panRight = std::sin(PI_OVER_FOUR * (1.0f + panning));
}

void Voice::setSampleRate(float sampleRate)
{
    env.setSampleRate(sampleRate);
    oscillator.sampleRate = sampleRate;
    oscillator2.sampleRate = sampleRate;
}
