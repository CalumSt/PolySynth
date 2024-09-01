/************************************************************************
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

* @file ADSREnvelope.h
* @author CS Islay
* @class ADSREnvelope
* @brief A class representing an ADSR envelope and related functionality.
* ADSREnvelope is a state machine to handle the different stages.
* 
************************************************************************/

#pragma once
#include <cmath>
#include "JuceHeader.h"
const float SILENCE = 0.0001f;

class ADSREnvelope
{
public:

    /**
     * @brief Calculates the next value of the envelope.
     * @return The next value of the envelope.
     */
    float nextValue()
    {
        // a one pole for smoothing
        level = multiplier *(level - target) + target;

        if (level + target > 3.0f)
        {
            multiplier = decayMultiplier;
            target = sustainLevel;
        }

        return level;   
    }

    void reset()
    {
        level = 0.0f;
        target = 0.0f;
        multiplier = 0.0f;

        attackMultiplier = 0.0f;
        decayMultiplier = 0.0f;
        sustainLevel = 1.0f;
        releaseMultiplier = 1.0f;

        inverseSampleRate = 0.0f;
        sampleRate = 0.0f;
    }

    void release()
    {
        target = 0.0f;
        multiplier = releaseMultiplier;
    }

    inline bool isActive() const
    {
        return level > SILENCE;
    }

    inline bool isInAttack() const
    {
        return target >= 2.0f;
    }

    void attack()
    {
        level += SILENCE + SILENCE;
        target = 2.0f;
        multiplier = attackMultiplier;
    }

    void setAttack(float normalisedAttack)
    {
        attackMultiplier = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * normalisedAttack));
    }

    void setDecay(float normalisedDecay)
    {
        decayMultiplier = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * normalisedDecay));
    }

    void setSustain(float normalisedSustain)
    {
        sustainLevel = normalisedSustain / 100.0f;
    }

    void setRelease(float normalisedRelease)
    {
        if (normalisedRelease < 1.0f) {
            releaseMultiplier = 0.75f; // extra fast release
        } else {
            releaseMultiplier = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * normalisedRelease));
        }


    }

    void setSampleRate(const float currentSampleRate)
    {
        sampleRate = currentSampleRate;
        inverseSampleRate = 1.0f / currentSampleRate;
    }

    float level = 0.0f; /**<The current level of the envelope. */
    float sampleRate = 44100.0f; /**<The sample rate of the signal the envelope is being applied to */

    // ADSR 
    float attackMultiplier = 0.0f;
    float decayMultiplier = 0.0f;
    float sustainLevel = 1.0f;
    float releaseMultiplier = 0.0f;

private:
    float multiplier = 0.0f; /**<The multiplier used to calculate the next value. */
    float target = 0.0f; /**<The target value of the envelope. */
    float inverseSampleRate = 1.0f / sampleRate;

};
