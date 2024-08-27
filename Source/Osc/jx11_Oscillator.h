/*****************************************************************************
  ,ad8888ba,    88        88  88  88      888888888888  ad88888ba   
 d8"'    `"8b   88        88  88  88           88      d8"     "8b  
d8'        `8b  88        88  88  88           88      Y8,          
88          88  88        88  88  88           88      `Y8aaaaa,    
88          88  88        88  88  88           88        `"""""8b,  
Y8,    "88,,8P  88        88  88  88           88              `8b  
 Y8a.    Y88P   Y8a.    .a8P  88  88           88      Y8a     a8P  
  `"Y8888Y"Y8a   `"Y8888Y"'   88  88888888888  88       "Y88888P"   

   _____   __ __   __  
  |_  \ \ / //  | /  | 
    | |\ V / `| | `| | 
    | |/   \  | |  | | 
/\__/ / /^\ \_| |__| |_
\____/\/   \/\___/\___/

 * @file jx11_Oscillator.h
 * @author CS Islay
 * @brief Base class for other oscillators.
 *
 * Oscillator class that handles different waveforms with an enumeration.
 * Based on Will Pirkle's book.
 * 
 ****************************************************************************

#pragma once
#include <math.h>
#include "Oscillator.h"

// Supported waveforms
enum Waveform
{
    sawtooth,
    sine,
    triangle,
    square,
    sawtoothReversed,
    squareReversed
};

class jx11_Oscillator : public Oscillator
{
public:
    float amplitude = 1.0f;
    float period = 0.0f;
    float sampleRate;

    void setWaveform(Waveform waveform)
    void reset() override
    auto nextSample() override

private:  
    float phaseInc;
    float phase;
    float phaseMax;
    float dc;
    float saw;

    Waveform waveform;
    // BLIT waveform generators, called by nextSample and are not meant to be called directly
    auto getSawtoothSample()
    auto getTriangleSample()
    auto getSquareSample()
    auto getSineSample()

    
}
*/
