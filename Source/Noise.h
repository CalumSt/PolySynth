#pragma once

// Very basic random number generator using properties of integers
// Probably should replace this with Juce's random number generator.

class Noise
{
    public:
        void reset()
        {
            noiseSeed = 22222;
        }

        float nextValue()
        {
            noiseSeed = noiseSeed * 196314165 + 908633515;
            int temp = int(noiseSeed >> 7) - 16777216;
            return float(temp) / 16777216.0f;
        }

    private:
    unsigned int noiseSeed;
};