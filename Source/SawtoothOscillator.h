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
        
        void reset() override
        {
            phase = 0.0f;
        }

        float nextSample() override
        {
            // Replaced with a phase counter which is set by an input frequency and sample rate.
            // This can go to higher pitches without falling to bits.
            // TODO: replace with BLIT or BLEP
            phase += inc;
            if (phase >= 1.0f) {
                phase -= 1.0f;
            }
            return amplitude * sinf(TWO_PI * phase);
        };
};