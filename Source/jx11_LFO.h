#pragma once

#include "Constants.h"
#include "SineOscillator.h"
constexpr int LFO_MAX = 32;

class jx11_LFO
/**
 * LFO (Low Frequency Oscillator)
 *
 * This class provides a Low Frequency Oscillator (LFO) for the synthesizer.
 * An LFO is an oscillator that generates a signal with a frequency below
 * the range of human hearing (usually below 20 Hz). This signal is then
 * used to modulate other components of the synthesizer, such as the pitch
 * or amplitude of an oscillator.
 *
 * The LFO provides a periodic signal, the shape of which can be
 * controlled by the user. The LFO can generate a sine, triangle, sawtooth,
 * or square wave, and the user can adjust the frequency and amplitude of
 * the LFO to achieve the desired effect.
 */

// TODO: add Doxygen comments
{
public:
    void reset()
    {
        lfoStep = 0;
        lfoInc = 0.0f;
        lfoRate = 0.0f;
    }

    void setSampleRate(float newSampleRate)
    {
        sampleRate = newSampleRate;
        inverseUpdateRate = LFO_MAX / newSampleRate;
    }

    void setLfoRate (const float rate)
    {
        lfoRate = std::exp(7.0f * rate - 4.0f);
        lfoInc = lfoRate * inverseUpdateRate * TWO_PI;
    }

    void setVibrato (const float _vibrato) { vibrato = _vibrato; }

    float render()// TODO: Comment me!
    {
        if (--lfoStep <= 0)
        {
            lfoStep = LFO_MAX;

            lfoPhase += lfoInc;
            if (lfoPhase > PI)
            {
                lfoPhase -= TWO_PI;
            }
            const float sine = std::sin (lfoPhase);
            lfoOutput = 1.0f + sine * vibrato;
        }
        return lfoOutput;
    }

private:
    float sampleRate = 44100.0f;
    float inverseUpdateRate = 1 * LFO_MAX / sampleRate;
    float lfoPhase = 0.0f;
    float lfoOutput = 1.0f;
    int lfoStep = LFO_MAX;
    float lfoInc = 0.0f;
    float lfoRate = 0.0f;
    float vibrato = 0.0f;
};