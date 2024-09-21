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