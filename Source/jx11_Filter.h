#pragma once
#include <cmath>
#include "Constants.h"
#include <iostream>
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

* @file jx11_Filter.h
* @author CS Islay
* @class jx11_Filter
* @brief A class implements a two-pole State Variable Filter (SVF) using the Cytomic filter design.
*
************************************************************************/

class jx11_Filter
{
public:
    void setSampleRate (const float _sampleRate) { sampleRate = _sampleRate; }
    [[nodiscard]] float getSampleRate() const { return sampleRate; }

    /**
     * @brief Updates the filter coefficients based on the cutoff frequency and quality factor.
     *
     * @param cutoff The cutoff frequency of the filter.
     * @param Q The quality factor of the filter.
     */
    void updateCoefficients(float cutoff, float Q)
    {
        g = std::tan (PI * cutoff / sampleRate);
        k = 1.0f / Q;
        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    /**
     * @brief Resets the filter coefficients and internal state variables to their default values.
     */
    void reset()
    {
        g = 0.0f;
        k = 0.0f;
        a1 = 0.0f;
        a2 = 0.0f;
        a3 = 0.0f;

        ic1eq = 0.0f;
        ic2eq = 0.0f;
    }

    /**
     * @brief Processes an input sample and produces a filtered output.
     *
     * @param x The input sample.
     * @return The filtered output sample.
     */
    float render(float x)
    {
        float v3 = x - ic1eq;
        float v1 = a1 * ic1eq + a2 * v3;
        float v2 = ic2eq + a2 * ic1eq + a3 * v3;
        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;
        return v2;
    }

private:
    float sampleRate = 44100.0f; ///< The sample rate of the filter

    float g = 0.0f; ///< The normalized angular frequency coefficient.

    float k = 0.0f; ///< The damping coefficient, inversely related to the quality factor.

    float a1 = 0.0f; ///< Coefficient a1 used in the filter difference equations.
    float a2 = 0.0f; ///< Coefficient a2 used in the filter difference equations.
    float a3 = 0.0f; ///< Coefficient a3 used in the filter difference equations.

    float ic1eq = 0.0f; ///< Internal state variable for the first integrator.
    float ic2eq = 0.0f; ///< Internal state variable for the second integrator.
};