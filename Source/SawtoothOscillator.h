/*****************************************************************************
*   ,ad8888ba,    88        88  88  88      888888888888  ad88888ba   
*  d8"'    `"8b   88        88  88  88           88      d8"     "8b  
* d8'        `8b  88        88  88  88           88      Y8,          
* 88          88  88        88  88  88           88      `Y8aaaaa,    
* 88          88  88        88  88  88           88        `"""""8b,  
* Y8,    "88,,8P  88        88  88  88           88              `8b  
*  Y8a.    Y88P   Y8a.    .a8P  88  88           88      Y8a     a8P  
*   `"Y8888Y"Y8a   `"Y8888Y"'   88  88888888888  88       "Y88888P"   
*
*    _____   __ __   __  
*   |_  \ \ / //  | /  | 
*     | |\ V / `| | `| | 
*     | |/   \  | |  | | 
* /\__/ / /^\ \_| |__| |_
* \____/\/   \/\___/\___/
*
* @file SawtoothOscillator.h
* @author CS Islay
* @brief A Class representing a sawtooth oscillator.
* 
* This class generates a sawtooth waveform using a Band-Limited Impulse Train (BLIT).
* @see https://ccrma.stanford.edu/~stilti/papers/blit.pdf for more information.
*
*****************************************************************************/



#pragma once
#include <cmath>
#include "Oscillator.h"

/**
* @class SawtoothOscillator
* @brief A sawtooth oscillator class that generates a sawtooth waveform using a BLIT.
* 
*/

class SawtoothOscillator : public Oscillator
{
    public:
        float amplitude = 1.0f;
        float period = 0.0f;
        float sampleRate;
        
        void reset() override
        {
            phase = 0.0f;
            inc = 0.0f;
            dc = 0.0f;
        }

        /**
         * @brief Generates the next sample of the sawtooth waveform using a Band-Limited Impulse Train (BLIT) approach.
         * 
         * This method uses a BLIT to generate a sawtooth waveform. It calculates the phase, increment, and DC offset of the waveform based on the period and amplitude.
         * 
         * @return The next sample of the sawtooth waveform.
         */
        float nextSample() override
        {
            float output = 0.0f;
            phase += inc;
            if (period != 0.0f) {
                // if phase goes over Pi/4, start a new impulse
                if (phase <= PI_OVER_FOUR) {
                    float halfPeriod = period / 2.0f; // find midpoint between last impulse and next
                    phaseMax = std::floor(0.5f + halfPeriod) - 0.5f; // This is stored in phaseMax
                    dc = 0.5f * amplitude / phaseMax; // calculate dc offset
                    phaseMax *= PI;
                    // update inc and phase member variables
                    inc = phaseMax / halfPeriod;
                    phase = -phase;
                    // Calculate the sinc function output (avoid dividing by zero)
                    if (phase*phase > 1e-9) {
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
                // convert the BLIT into a saw wave with a leaky integrator (adding up inputs over time)
                saw = saw * 0.997f + output - dc;
            } else {
                saw = -120.0f;
            }
            return saw;
        };

    private:
        float phase;
        float phaseMax;
        float inc;
        float dc;
        float saw;
        
};