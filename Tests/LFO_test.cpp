//
// Created by calum on 15/09/2024.
//
#pragma once
#include <gtest/gtest.h>
#include "jx11_LFO.h"

TEST(LfoTest, render_test)
{
    jx11_LFO LFO;
    auto nextSample = LFO.render();
    EXPECT_EQ(0.0f,nextSample);
}

TEST(LfoTest, renderOneSecond_test)
{
    jx11_LFO LFO;
    LFO.setSampleRate (44100.f);
    LFO.setLfoRate (1.0f);
    constexpr int numberOfSamples = 44100;
    for (int i = 0; i < numberOfSamples; i++)
    {
        auto nextSample = LFO.render();
        EXPECT_NE(0.0f,nextSample);
        EXPECT_NEAR(1.0f, nextSample,0.1f);
    }

}