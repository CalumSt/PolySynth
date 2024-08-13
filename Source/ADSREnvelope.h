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
* 
************************************************************************/

#pragma once
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
        level *= multiplier *(level - target) + target;

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

    float level; /**<The current level of the envelope. */

    // ADSR
    float attackMultiplier;
    float decayMultiplier;
    float sustainLevel;
    float releaseMultiplier;

private:
    float multiplier; /**<The multiplier used to calculate the next value. */
    float target; /**<The target value of the envelope. */

};
