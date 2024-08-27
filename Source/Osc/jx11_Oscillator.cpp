#pragma once
/*
#include "Oscillator.h"


void Oscillator::setWaveform(enum Waveform);
void Oscillator::reset();
auto Oscillator::nextSample();

auto Oscillator::getSawtoothSample()
{
    float output = getUnipolarBlitSample();
    // convert the BLIT into a saw wave with a leaky integrator (adding up inputs over time)
    // This needs some additional thought as it requires access to the previous sample, which will need saved somewhere (for each waveform?)
    saw = saw * 0.997f + output - dc;
    return saw;
}

auto Oscillator::getTriangleSample()
{
    float output = getBipolarBlitSample()
    // convert the Bipolar BLIT to a triangle wave by leaky integration
    triangle = triangle * 0.997f + output - dc;
}

auto Oscillator::getUnipolarBlitSample()
{
float output = 0.0f;
phase += phaseInc;

float halfPeriod = period / 2.0f; // find midpoint between last impulse and next
phaseMax = std::floor(0.5f + halfPeriod) - 0.5f; // This is stored in phaseMax
dc = -0.5f * amplitude / phaseMax; // calculate dc offset

// if phase goes over Pi/4, start a new impulse
if (phase <= PI_OVER_FOUR) {
    phaseMax *= PI;
    // update inc and phase member variables
    phaseInc = phaseMax / halfPeriod;
    phase = -phase;
    // Calculate the sinc function output (avoid dividing by zero)
    if (phase*phase > 1e-9) {
        output = dc + amplitude*sin(phase) / phase;
    } else {
        output = dc + amplitude;
    };

} else { // If between peaks of impulses

    if (phase > phaseMax) { // invert increment and loop back through the sinc function
        phase = phaseMax + phaseMax - phase;
        phaseInc = -phaseInc;
    }
    // calculate the sinc function output - don't need to worry about divide by 0 here
    output = dc + amplitude * sin(phase) / phase;
};
}

auto Oscillator::getBipolarBlitSample()
{
// Similar to unipolarBlitSample, but applies a - sign to the output if it's between inpulses
float output = 0.0f;
phase += phaseInc;
float correctionFactor;

// if phase goes over Pi/4, start a new impulse
if (phase <= PI_OVER_FOUR) {
    correctionFactor = 1.0f;
    float halfPeriod = period / 2.0f; // find midpoint between last impulse and next
    phaseMax = std::floor(0.5f + halfPeriod) - 0.5f; // This is stored in phaseMax
    phaseMax *= PI;
    // update inc and phase member variables
    phaseInc = phaseMax / halfPeriod;
    phase = -phase;
    // Calculate the sinc function output (avoid dividing by zero)
    if (phase*phase > 1e-9) {
        output = amplitude*sin(phase) / phase;
    } else {
        output = amplitude;
    };

} else { // If between peaks of impulses
    correctionFactor = -1.0f;
    if (phase > phaseMax) { // invert increment and loop back through the sinc function
        phase = phaseMax + phaseMax - phase;
        phaseInc = -phaseInc;
    }
    // calculate the sinc function output - don't need to worry about divide by 0 here
    output = correctionFactor*amplitude * sin(phase) / phase;
};
}
*/