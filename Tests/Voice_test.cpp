#pragma once
#include <gtest/gtest.h>
#include <stdio.h>
#include "Voice.h"

// helper functions
float calculatePeriodFromNote(float noteNumber, float sampleRate)
{
    float freq = std::exp2((noteNumber - 69.0f) / 12.0f) * 440.0f;
    float period = sampleRate / freq;
    return period;
}

// Helper function to setup the oscillator

// Arrange, Act and Assert
TEST(VoiceTests, Constructor_test) {
    Voice voice;
    voice.reset();
    EXPECT_EQ(0.0f, voice.note);
    EXPECT_EQ(0.0f, voice.velocity);
    EXPECT_EQ(0.707f, voice.panLeft);
    EXPECT_EQ(0.707f, voice.panRight);
}

TEST(VoiceTests, noteOn_test) {
// stub test
}

TEST(VoiceTests, update_test) {
    int note = 60;

    Voice voice;
    voice.reset();
    voice.note = note;
    voice.update();
    
    float panning = std::clamp((note - 60.0f) / 24.0f, -1.0f, 1.0f); // notes outside this range are clamped
    auto CORRECT_VALUE_LEFT = std::sin(PI_OVER_FOUR * (1.0f - panning));
    auto CORRECT_VALUE_RIGHT = std::sin(PI_OVER_FOUR * (1.0f + panning));

    EXPECT_EQ(CORRECT_VALUE_LEFT, voice.panLeft);
    EXPECT_EQ(CORRECT_VALUE_RIGHT, voice.panRight);
    
}

TEST(VoiceTests,render_test) {

    Voice voice;
    voice.reset();
    voice.note = 60;
    voice.velocity = 127;
    voice.panLeft = 0.5f;
    voice.panRight = 0.5f;

    voice.setSampleRate(44100.0f);
    voice.env.setAttack(50.0f);
    voice.env.setDecay(50.0f);
    voice.env.setSustain(50.0f);
    voice.env.setRelease(50.0f);
    voice.env.attack();

    voice.oscillator.amplitude = 0.5f;
    voice.oscillator2.amplitude = 0.5f;
    float period = calculatePeriodFromNote(60.0f, 44100.0f);
    std::cout << period;
    voice.oscillator.period = period;
    voice.oscillator2.period = period;


    voice.filter.reset();
    voice.filter.setSampleRate (44100.0f);
    voice.filter.updateCoefficients(1000.0f,0.707f);

    auto sample = voice.render(0.5f);
    EXPECT_GT(sample, 0.0f);

    float numberOfSamples = 1000;
    for (int i = 0; i < numberOfSamples; i++) {

        auto testSample = voice.render(0.0f);

        EXPECT_NE(0.0f, testSample);
        EXPECT_GT (-1.0f, testSample);
        EXPECT_LT(1.0f, testSample);
    };
}