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
 * Oscillator.h
 * 
 * A Class representing a synthesiser oscillator.
 * 
 * CS Islay
 ******************************************************************/


#pragma once
#include <math.h>
#include "Oscillator.h"

class SawtoothOscillator : public Oscillator
{
    public:
        float amplitude;
        float inc;
        float phase;

        float frequency;
        float sampleRate;
        float phaseBandlimited;
        
        void reset() override
        {
            phase = 0.0f;
            phaseBandlimited = 0.0f;
        }

        float nextBandLimitedSample()
        {
            // Naive Sawtooth
            // TODO: replace with BLIT or BLEP
            phaseBandlimited += inc;
            if (phaseBandlimited >= 1.0f) {
                phaseBandlimited -= 1.0f;
            }

            float output = 0.0f;
            float nyquist = sampleRate / 2.0f;
            float h = frequency;
            float harmonicIndex = 1.0f;
            auto scalingFactor = TWO_OVER_PI;

            while (h < nyquist && h>= 0) {
                output += scalingFactor * sinf(TWO_PI * phaseBandlimited * harmonicIndex) / harmonicIndex;
                h += frequency;
                harmonicIndex += 1.0f;
                scalingFactor = -scalingFactor;
            };
            return output;
        };

        float nextSample() override
        {
            return amplitude * nextBandLimitedSample();
        };

};