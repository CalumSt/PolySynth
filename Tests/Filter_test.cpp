#pragma once
#include <gtest/gtest.h>
#include "jx11_Filter.h"
#include "jx11_Oscillator.h"
#include "Helpers.h"

// How do you test a filter?
// If you used an oscillator to generate a test signal, how do you predict the filter characteristics?
// You could have a reference filtered signal - say generated in Python

// Helper function to setup the oscillator
jx11_Oscillator setupOsc() {
    jx11_Oscillator osc;
    osc.reset();
    osc.sampleRate = 44100.0f;
    osc.amplitude = 0.5f;
    float period = calculatePeriod(69.0f, 44100.0f);
    osc.period = period;
    return osc;
}

TEST(FilterTests,Constructor_test)
{
    jx11_Filter filter;
    const auto sample = filter.render(1.0f);
    EXPECT_EQ (sample, 0.0f);
}

TEST(FilterTests, Filter_test)
{
    jx11_Filter filter;
    filter.reset();
    filter.setSampleRate (44100.0f);
    filter.updateCoefficients(1000.0f,0.707f);
    jx11_Oscillator osc = setupOsc();
    int numberOfSamples = 44100; // 1 second of samples
    for (int i = 0; i < numberOfSamples; i++) {
        const float oscSample = osc.render();
        EXPECT_GT(oscSample, -1.0f);
        EXPECT_LT(oscSample, 1.0f);
        const float nextValue = filter.render(oscSample);
        EXPECT_GT(nextValue, -1.0f);
        EXPECT_LT(nextValue, 1.0f);
        EXPECT_NE (oscSample, nextValue);
    }
}