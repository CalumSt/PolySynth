/*****************************************************************************
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

 * @file Oscillator.h
 * @author CS Islay
 * @brief Base class for other oscillators.
 *
 * This class provides a common interface for different types of oscillators.
 * The idea is that you should be able to call them via enumeration, but I don't
 * know if that would work for changing oscillators mid note.
 * It may be better to have one class for different types of synthesis, so that
 * calling different methods calls a different waveform generation.
 * 
 ****************************************************************************/
#pragma once
#include "Constants.h"

class Oscillator {
    public:
        Oscillator() = default;
        virtual ~Oscillator() = default;
        virtual void reset() = 0;
        virtual float nextSample() = 0;

};