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
 * SineOscillator.h
 * 
 * A Class representing a synthesiser oscillator that uses a sine wave
 * Ideally want to combine this with other oscillator classes
 * 
 * CS Islay
 ******************************************************************/


#pragma once
#include <math.h>
#include "Oscillator.h"

class SineOscillator : public Oscillator
{
    public:
        float amplitude;
        float inc;
        float phase;
        float sin0; // TODO: think of a better name for these
        float sin1;
        float dsin;
        
        
        void reset() override
        {
            sin0 = amplitude * sinf(phase * TWO_PI);
            sin1 = amplitude * sinf((phase-inc) * TWO_PI);
            dsin = 2.0f * cosf(inc*TWO_PI);
        }

        float nextSample() override
        {
            // Replaced with a Direct Form Digital Resonator
            // Not too happy with this as I don't know how this works
            // TODO: replace with BLIT or BLEP
            auto sinx = dsin * sin0 + sin1;
            sin1 = sin0;
            sin0 = sinx;
            return amplitude * sinf(TWO_PI * phase);
        };
};