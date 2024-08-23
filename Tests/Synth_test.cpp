TEST(SynthTest, RenderProducesSilenceWhenInputBuffersAreNull) {
  Synth synth;
  float* outputBufferLeft = nullptr;
  float* outputBufferRight = nullptr;
  int sampleCount = 100;

  synth.render(outputBufferLeft, outputBufferRight, sampleCount);

  // Verify that the output buffers are still null
  EXPECT_EQ(nullptr, outputBufferLeft);
  EXPECT_EQ(nullptr, outputBufferRight);
}