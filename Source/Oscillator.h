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

const auto PI = atanf(1.f) * 4;
const auto TWO_PI = 2 * PI;

class Oscillator
{
    public:
        float amplitude;
        float frequency;
        float sampleRate;
        float phaseOffset;
        int sampleIndex;
        
        void reset()
        {
            sampleIndex = 0;
        }

        float nextSample()
        {
            // Literally use sin equation for wave, increase sampleIndex, and return the calculated sample.
            // TODO: replace with BLIT or BLEP
            float output = amplitude * sinf(TWO_PI * sampleIndex * frequency / sampleRate+phaseOffset);
            sampleIndex += 1;
            return output;
        };
};