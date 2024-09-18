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
* @class jx11_Oscillator
* @brief An oscillator class that generates a sawtooth waveform using a BLIT.
* 
*/

class jx11_Oscillator : public Oscillator
{
    public:
        float amplitude = 1.0f;
        float modulation = 1.0f;
        float period = 0.0f;
        float sampleRate = 44100.f;
        
        void reset() override
        {
            phase = 0.0f;
            inc = 0.0f;
            dc = 0.0f;
            saw = 0.0f;
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
            // if phase goes over Pi/4, start a new impulse
            if (phase <= PI_OVER_FOUR) {
                const float halfPeriod = (period / 2.0f) * modulation; // find midpoint between last impulse and next
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
                }

            } else { // If between peaks of impulses

                if (phase > phaseMax) { // invert increment and loop back through the sinc function
                    phase = phaseMax + phaseMax - phase;
                    inc = -inc;
                }
                // calculate the sinc function output - don't need to worry about divide by 0 here
                output = amplitude * sin(phase) / phase;
            }
            return output - dc;
        };

        float render()
        {
            const float sample = nextSample();
            // convert the BLIT into a saw wave with a leaky integrator (adding up inputs over time)
            saw = saw * 0.997f + sample;
            return sample;
        };

    float getNextSquareSample()
    {
        // Similar to unipolarBlitSample, but applies a - sign to the output if it's between inpulses
        float output = 0.0f;
        phase += inc;
        float correctionFactor;

        // if phase goes over Pi/4, start a new impulse
        if (phase <= PI_OVER_FOUR) {
            correctionFactor = 1.0f;
            float halfPeriod = period / 2.0f; // find midpoint between last impulse and next
            phaseMax = std::floor(0.5f + halfPeriod) - 0.5f; // This is stored in phaseMax
            phaseMax *= PI;
            // update inc and phase member variables
            inc = phaseMax / halfPeriod;
            phase = -phase;
            // Calculate the sinc function output (avoid dividing by zero)
            if (phase*phase > 1e-9) {
                output = amplitude*sin(phase) / phase;
            } else {
                output = amplitude;
            }

        } else { // If between peaks of impulses
            correctionFactor = -1.0f;
            if (phase > phaseMax) { // invert increment and loop back through the sinc function
                phase = phaseMax + phaseMax - phase;
                inc = -inc;
            }
            // calculate the sinc function output - don't need to worry about divide by 0 here
            output = amplitude * sin(phase) / phase;
        }

        return correctionFactor * output - dc;
    }

    private:
        float phase = 0;
        float phaseMax = 0;
        float inc = 0;
        float dc = 0;
        float saw = 0;
        float square = 0;

        void squareWave(jx11_Oscillator const& other, const float newPeriod)
        {
            reset();

            if (other.inc > 0.0f)
            {
                phase = other.phaseMax + other.phaseMax - other.phase;
                inc = -other.inc;
            } 
            else if (other.inc < 0.0f)
            {
                phase = other.phase;
                inc = other.inc;                
            }
            else 
            {
                phase = -PI;
                inc = PI;
            }

            phase += PI * newPeriod / 2.0f;
            phaseMax = phase;
        }
        
};