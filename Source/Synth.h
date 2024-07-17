/*
  ,ad8888ba,    88        88  88  88      888888888888  ad88888ba   
 d8"'    `"8b   88        88  88  88           88      d8"     "8b  
d8'        `8b  88        88  88  88           88      Y8,          
88          88  88        88  88  88           88      `Y8aaaaa,    
88          88  88        88  88  88           88        `"""""8b,  
Y8,    "88,,8P  88        88  88  88           88              `8b  
 Y8a.    Y88P   Y8a.    .a8P  88  88           88      Y8a     a8P  
  `"Y8888Y"Y8a   `"Y8888Y"'   88  88888888888  88       "Y88888P"   
*/

/*
   _____   __ __   __  
  |_  \ \ / //  | /  | 
    | |\ V / `| | `| | 
    | |/   \  | |  | | 
/\__/ / /^\ \_| |__| |_
\____/\/   \/\___/\___/
*/

/******************************************************************
 * Synth.h
 * Synth.cpp
 * 
 * A Class representing a Synthesiser, which interfaces with the 
 * audio processor.
 * 
 * CS Islay
 ******************************************************************/

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