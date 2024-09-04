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
* @file Synth.h
* @author CS Islay
* @brief A Class representing a Synthesiser, which interfaces with the 
*        audio processor.
******************************************************************/

#pragma once

#include <JuceHeader.h>
#include "Voice.h"
#include "Noise.h"

/**
 * @class Synth
 * @brief A synthesizer class that interfaces with the audio processor.
 */
class Synth
{
    public:
    
        static constexpr int MAX_VOICES = 8;

        /**
         * @brief Default constructor.
         */
        Synth();

        /**
         * @brief Allocates resources for the synthesizer.
         * @param sampleRate The sample rate of the audio processor.
         * @param samplesPerBlock The number of samples per block.
         */
        void allocateResources(double sampleRate, int samplesPerBlock);

        /**
         * @brief Deallocates resources for the synthesizer.
         */
        void deallocateResources();

        /**
         * @brief Resets the synthesizer to its default state.
         */
        void reset();

        /**
         * @brief Renders audio output.
         * @param outputBuffers The output buffers to render to.
         * @param sampleCount The number of samples to render.
         */
        void render(float** outputBuffers, int sampleCount);

        /**
         * @brief Processes MIDI messages.
         * @param data0 The first byte of the MIDI message.
         * @param data1 The second byte of the MIDI message.
         * @param data2 The third byte of the MIDI message.
         */
        void midiMessages(uint8_t data0, uint8_t data1, uint8_t data2);

        /**
         * @brief The voices used to hold note, oscillators and evelopes
         */
        std::array<Voice, MAX_VOICES> voices;

        int numVoices;

        float noiseMix;

        /**
         * @brief The decay time of the envelope.
         * @note Range: 0.0 (no decay) to 10.0 (maximum decay)
         */
        float envDecay;

        /**
         * @brief The attack time of the envelope.
         * @note Range: 0.0 (no attack) to 10.0 (maximum attack)
         */
        float envAttack;

        /**
         * @brief The sustain level of the envelope.
         * @note Range: 0.0 (no sustain) to 1.0 (maximum sustain)
         */
        float envSustain;

        /**
         * @brief The release time of the envelope.
         * @note Range: 0.0 (no release) to 10.0 (maximum release)
         */
        float envRelease;

        // Oscillator parameters
        /**
         * @brief The mix level of the oscillator.
         * @note Range: 0.0 (no mix) to 1.0 (maximum mix)
         */
        float oscMix;
        /**
         * @brief The detune amount of the oscillator.
         * @note Range: -1.0 (maximum detune) to 1.0 (maximum detune)
         */
        float detune;
        /**
         * @brief The fine tuning of the oscillator.
         * @note Range: -1.0 (maximum fine tune) to 1.0 (maximum fine tune)
         */
        float oscFine;

        // tuning and pitch bend
        /**
         * @brief The overall tuning of the synth.
         * @note Range: -1.0 (maximum detune) to 1.0 (maximum detune)
         */
        float tune;
        /**
         * @brief The pitch bend amount of the synth.
         * @note Range: -1.0 (maximum pitch bend) to 1.0 (maximum pitch bend)
         */
        float pitchBend;

        // make documentation for these: make sure it's clear that these are for setting the parameters from a percentage

        void setAttack(float attackPercentage);
        void setDecay(float decayPercentage);
        void setSustain(float sustainPercentage);       
        void setRelease(float releasePercentage);

    private:
        float sampleRate;
        float inverseSampleRate;
        Noise noise;
        void noteOn(int note,int velocity);
        void startVoice(int voiceIndex, int note, int velocity);
        void noteOff(int note);
        float calculatePeriod(int note) const;
};