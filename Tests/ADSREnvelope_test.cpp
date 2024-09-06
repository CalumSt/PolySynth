#pragma once
#include <gtest/gtest.h>
#include "ADSREnvelope.h"

float sampleRate = 44100.0f;
float inverseSampleRate = 1/sampleRate;


TEST(ADSR_tests,resetSetsParametersCorrectly_test) 
{
    ADSREnvelope env;
    env.level = 0.5f;
    env.reset();
    EXPECT_EQ(env.level, 0.0f);
}

TEST(ADSR_tests,setSampleRate_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    EXPECT_EQ(env.sampleRate, sampleRate);
}

TEST(ADSR_tests,setAttack_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setAttack(50.0f);
    auto CORRECT_VALUE = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * 50.0f));
    EXPECT_EQ(env.attackMultiplier, CORRECT_VALUE);
}


TEST(ADSR_tests,setDecay_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setDecay(50.0f);
    auto CORRECT_VALUE = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * 50.0f));
    EXPECT_EQ(env.decayMultiplier, CORRECT_VALUE);

}

TEST(ADSR_tests,setSustain_test)
{   
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setSustain(0.5f);
    auto CORRECT_VALUE = 0.5f/100.0f;
    EXPECT_EQ(env.sustainLevel, CORRECT_VALUE);
}

TEST(ADSR_tests,setRelease_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setRelease(0.5f);
    auto CORRECT_VALUE = 0.75f;
    EXPECT_EQ(env.releaseMultiplier, CORRECT_VALUE);

    env.setRelease(1.0f);
    auto NEW_CORRECT_VALUE = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * 1.0f));
    EXPECT_EQ(env.releaseMultiplier, NEW_CORRECT_VALUE);
}

TEST(ADSR_tests,attackStage_test)
{

    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.attackMultiplier = 0.5f;
    env.attack();
    EXPECT_EQ(env.level,SILENCE+SILENCE);
}

TEST(ADSR_tests,releaseStage_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setAttack(50.0f);
    env.setDecay(50.0f);
    env.setSustain(50.0f);
    env.setRelease(50.0f);
    env.attack();
    env.release();
    EXPECT_EQ(env.level,SILENCE+SILENCE);
}

TEST(ADSR_tests,nextValue_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setAttack(50.0f);
    env.setDecay(50.0f);
    env.setSustain(50.0f);
    env.setRelease(50.0f);
    env.attack();
    float nextValue = env.nextValue();

    // Expect that the next value is between -1.0 and 1.0

    EXPECT_NE(nextValue,0.0f);
    EXPECT_GT(nextValue,-1.0f);
    EXPECT_LT(nextValue,1.0f);
}


TEST(ADSR_tests,testFullEnvelope_test)
{
    ADSREnvelope env;
    env.reset();
    env.setSampleRate(sampleRate);
    env.setAttack(50.0f);
    env.setDecay(50.0f);
    env.setSustain(50.0f);
    env.setRelease(50.0f);
    env.attack();

    auto numberOfSamples = 1000;
    for (int i = 0; i < numberOfSamples; i++) {
        float nextValue = env.nextValue();
        EXPECT_GT(nextValue, 0.0f);
        EXPECT_LT(nextValue, 1.0f);
    }
}
