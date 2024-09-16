#pragma once

#include "Constants.h"
#include "SineOscillator.h"

class jx11_LFO : public SineOscillator
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
{
public:
    jx11_LFO()
    {
        amplitude = 1.0f;
        phase = 0.0f;
        inc = 0.0f;
        SineOscillator::reset();

    }
    const float LFO_MAX = 32.0f;

    void setFrequency (float frequency)
    {
        // Sets the frequency of the lfo in hertz, converting to the internal variables
        // This is done for intuitive interactions with the LFO
        // TODO: Model in python to set conversions
        if (frequency > LFO_MAX) { frequency = LFO_MAX; }
        // phase takes a value between 0 and 1, so need to convert to ft
        phase = frequency * inc;
    }


};