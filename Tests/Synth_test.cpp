#pragma once
#include <gtest/gtest.h>
#include "Synth.h"
#include "JuceHeader.h"

TEST(SynthTest, Constructor_test) {
  Synth synth;
  EXPECT_FLOAT_EQ(synth.sampleRate, 44100.0f);
}


TEST(SynthTest, NoteOn_test) {
  Synth synth = setupTest();
  synth.noteOn(60, 127);
  // This calls update() as well as env.attack() so need to make sure that these are done first
}



TEST(SynthTest, Render) {
  Synth synth;
  float outputBuffers[2][10] ;
  int sampleCount = 10;
  synth.render(outputBuffers, sampleCount);

  // Verify that the output buffers are not empty
  for (int i = 0; i < sampleCount; i++) {
    EXPECT_NE(outputBuffers[0][i], 0.0f);
    EXPECT_NE(outputBuffers[1][i], 0.0f);
  }
}

Synth setupTest() {
  Synth synth;
  synth.reset();
  // Set all the variables of the synth
  synth.envDecay = 0.1f;
  synth.envAttack = 0.1f;
  synth.envSustain = 0.5f;
  synth.envRelease = 0.1f;
  synth.oscMix = 0.5f;
  synth.detune = 0.0f;
  synth.oscFine = 0.0f;
  synth.tune = 0.0f;
  synth.pitchBend = 0.0f;

  return Synth
}

float* outputBuffers setupBuffer(int numSamples) {
  juce::AudioBuffer<float> buffer(2, numSamples);
  buffer.clear();
  float* outputBuffers[2] = { nullptr, nullptr };
  outputBuffers[0] = buffer.getWritePointer(0);
  outputBuffers[1] = buffer.getWritePointer(1);
  return outputBuffers
}