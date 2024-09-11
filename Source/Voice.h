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
* Voice.h
* 
* A Class representing a synthesiser voice. A voice holds 
* oscillators and note information.
* 
* CS Islay
*****************************************************************************/


#pragma once
#include "SineOscillator.h"
#include "jx11_Oscillator.h"
#include "ADSREnvelope.h"
#include <cmath>
#include <algorithm>

class Voice
{
    public:
        int note;
        int velocity;
        jx11_Oscillator oscillator;
        jx11_Oscillator oscillator2;
        ADSREnvelope env;
        float period;

        // panning
        float panLeft, panRight;

        // methods

        void noteOn(int note, int velocity);
        void noteOff();
        void reset();
        void update();
        float render(float input);
        void setSampleRate(float sampleRate);
};