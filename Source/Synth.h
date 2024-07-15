#pragma once

#include <JuceHeader.h>
#include "Voice.h"
#include "Noise.h"

class Synth
{
    public:
    Synth();

    void allocateResources(double sampleRate,int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render(float** outputBuffers, int sampleCount);
    void midiMessages(uint8_t data0,uint8_t data1,uint8_t data2);

    private:
    float sampleRate;
    Voice voice;
    Noise noise;
    void noteOn(int note,int velocity);
    void noteOff(int note);
};