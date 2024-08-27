#pragma once
#include <gtest/gtest.h>
#include "SawtoothOscillator.h"

// Arrange, Act and Assert

// Helper function to calculate the period for A440Hz
float calculatePeriod(float noteNumber)
{
    float period = std::exp(-0.05776226505f * noteNumber);
    return period;
}

// Helper function to setup the oscillator
SawtoothOscillator testSetup() {
    SawtoothOscillator osc;
    osc.reset();
    osc.sampleRate = 44100.0f;
    float period = calculatePeriod(69.0f);
    osc.period = period;
    return osc;
}

TEST(OscTests,setupCorrectlyTest)
{
    SawtoothOscillator osc;
    osc.reset();
    EXPECT_EQ(osc.amplitude,1.0f);
    EXPECT_EQ(osc.period,0.0f);

}

TEST(OscTests,nextSampleWorksCorrectlyTest)
{
    SawtoothOscillator osc = testSetup();
    auto Sample = osc.nextSample();
    EXPECT_NEAR(Sample,0.0f,0.01f);
}

// To properly test the sawtooth, we would need a reference signal.
TEST(OscTests,nextSampleProducesCorrectValueTest)
{   

    SawtoothOscillator osc;
    osc.reset();
    float period = calculatePeriod(69.0f);
    osc.period = period;
    auto Sample = osc.nextSample();

    auto CORRECT_VALUE = 0.0f;
    float amplitude = 1.0f;
    float phase = 0;
    float halfPeriod = period / 2.0f; // find midpoint between last impulse and next
    auto phaseMax = std::floor(0.5f + halfPeriod) - 0.5f;
    phaseMax *= PI;
    // update inc and phase member variables
    auto inc = phaseMax / halfPeriod;
    phase = -phase;
    // Calculate the sinc function output (avoid dividing by zero)
    if (phase*phase > 1e-9) {
        CORRECT_VALUE = amplitude*sin(phase) / phase;
    } else {
        CORRECT_VALUE = amplitude;
    };

    EXPECT_EQ(CORRECT_VALUE,Sample);
}


