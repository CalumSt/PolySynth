#pragma once
#include <gtest/gtest.h>
#include "Voice.h"

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

    voice.env.setAttack(0.5f);
    voice.env.setDecay(0.5f);
    voice.env.setSustain(0.5f);
    voice.env.setRelease(0.5f);

    float numberOfSamples = 1000;
    for (int i = 0; i < numberOfSamples; i++) {

        auto sample = voice.render(0.0f);

        EXPECT_NE(0.0f, sample);
    };
}