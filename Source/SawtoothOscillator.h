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
        float amplitude = 1.0f;
        float period = 0.0f;
        float sampleRate;
        
        void reset() override
        {
            phase = 0.0f;
            phaseBandlimited = 0.0f;
            inc   = 0.0f;
        }

        
        float nextNaiveSample()
        {
            return amplitude * nextBandLimitedSample();
        };

        float nextSample() override
        {
            float output = 0.0f;
            phase += inc;

            // if phase goes over Pi/4, start a new impulse
            if (phase <= PI_OVER_FOUR) {
                float halfPeriod = period / 2.0f; // find midpoint between last impulse and next
                phaseMax = std::floor(0.5f + halfPeriod) - 0.5f; // This is stored in phaseMax
                phaseMax *= PI;

                inc = phaseMax / halfPeriod;
                phase = -phase;

                if (phase*phase > 1e-9) { // avoid dividing by 0
                    output = amplitude*sin(phase) / phase;
                } else {
                    output = amplitude;
                };

            } else { // If between peaks of impulses

                if (phase > phaseMax) { // invert increment and loop back through the sinc function
                    phase = phaseMax + phaseMax - phase;
                    inc = -inc;
                }
                // calculate the sinc function output - don't need to worry about divide by 0 here
                output = amplitude * sin(phase) / phase;
            };

            return output;
        };

    private:
        float phase;
        float phaseMax;
        float inc;
        
        // This are just used for the bandlimited Naive below
        float incNaive;
        float frequency;
        float phaseBandlimited;
        // This is kept for posterity but isn't used
        float nextBandLimitedSample()
        {
            // Naive Sawtooth
            phaseBandlimited += incNaive;
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


};