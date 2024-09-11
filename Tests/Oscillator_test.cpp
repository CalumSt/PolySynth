#pragma once
#include <gtest/gtest.h>
#include "jx11_Oscillator.h"

// Arrange, Act and Assert

// Helper function to calculate the period for A440Hz
float calculatePeriod(float noteNumber, float sampleRate)
{
    float freq = std::exp2((noteNumber - 69.0f) / 12.0f) * 440.0f;
    float period = sampleRate / freq;
    return period;
}

// Helper function to setup the oscillator
jx11_Oscillator testSetup() {
    jx11_Oscillator osc;
    osc.reset();
    osc.sampleRate = 44100.0f;
    osc.amplitude = 0.5f;
    float period = calculatePeriod(69.0f, 44100.0f);
    osc.period = period;
    return osc;
}

TEST(OscTests,setup_test)
{
    jx11_Oscillator osc;
    osc.reset();
    EXPECT_EQ(osc.amplitude,1.0f);
    EXPECT_EQ(osc.period,0.0f);

}

TEST(OscTests,nextSample_test)
{
    jx11_Oscillator osc = testSetup();
    auto Sample = osc.nextSample();
    EXPECT_NE(Sample,0.0f);
    EXPECT_LT(Sample,1.0f);
    EXPECT_GT(Sample,-1.0f);
}

// To properly test the sawtooth, we would need a reference signal.
TEST(OscTests,nextSampleCorrectValue_test)
{   

    jx11_Oscillator osc = testSetup();
    auto Sample = osc.nextSample();
    float period = calculatePeriod(69.0f, 44100.0f);
    auto CORRECT_VALUE = 0.0f;
    float amplitude = 0.5f;
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

    EXPECT_NEAR(CORRECT_VALUE,Sample,0.01f);
}

TEST(OscTests,render_test)
{
    jx11_Oscillator osc = testSetup();
    auto Sample = osc.render();
    EXPECT_NE(Sample,0.0f);
    EXPECT_LT(Sample,1.0f);
    EXPECT_GT(Sample,-1.0f);
}


