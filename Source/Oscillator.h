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
const auto PI = atanf(1.f) * 4;
const auto TWO_PI = 2 * PI;
const auto TWO_OVER_PI = 2/PI;
const auto PI_OVER_FOUR = PI / 4;

class Oscillator {
    public:
        virtual void reset() = 0;
        virtual float nextSample() = 0;

};