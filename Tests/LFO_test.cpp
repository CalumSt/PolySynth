//
// Created by calum on 15/09/2024.
//
#pragma once
#include <gtest/gtest.h>
#include "jx11_LFO.h"

TEST(LfoTest, construction_test)
{
    jx11_LFO LFO;
    EXPECT_EQ (LFO.amplitude, 1.0f);
}

TEST(LfoTest, setFrequency_test)
{
    jx11_LFO LFO;
}