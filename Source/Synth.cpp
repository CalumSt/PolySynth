#pragma once
#include "Synth.h"
#include "Utils.h"



// This class defines the Synthesiser interface and rendering methods.
// 15/07/2024

Synth::Synth()
{
    sampleRate = 44100.0f;
}

void Synth::allocateResources(double sampleRate_,int samplesPerBlock)
{
    sampleRate = static_cast<float>(sampleRate_);
}

void Synth::deallocateResources()
{

}

void Synth::reset()
{
    for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex) 
    {
        voices[voiceIndex].reset();
    }

    noise.reset();
    pitchBend = 1.0f; // Give this a value as it isn't received if the user doesn't touch the pitch bend
    sustainPedalPressed = false;
}
// TODO: Get descriptions for these inputs
void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    // set up oscillator periods
    for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
    {
        Voice& voice = voices[voiceIndex];
        // if (voice.env.isActive()) 
        
        voice.oscillator.period = voice.period * pitchBend;
        voice.oscillator2.period = voice.period * detune;
        
    }
    // Loop through samples
    for (int sample = 0; sample < sampleCount; ++ sample) {
        // get next noise sample
        auto noiseSample = noise.nextValue() * noiseMix;

        // make sure note is being played, then apply velocity
        float outputSampleLeft = 0.0f;
        auto outputSampleRight = outputSampleLeft;

        for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
        {
            Voice& voice = voices[voiceIndex];
            if (voice.env.isActive()) 
            {
                // get next oscillator samples and pan
                float outputSample = voice.render(noiseSample);
                outputSampleLeft += outputSample * voice.panLeft;
                outputSampleRight += outputSample * voice.panRight;

                outputSampleLeft *= outputLevel;
                outputSampleRight *= outputLevel;
            }
        }
         // copy output to each channel, only applying to left if we're in mono
        if (outputBufferRight != nullptr) 
        {
            outputBufferLeft[sample] = outputSampleLeft;
            outputBufferRight[sample] = outputSampleRight;
        } else {
            outputBufferLeft[sample] = (outputSampleLeft + outputSampleRight) * 0.5f;
        }
    }
    for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
        {
            Voice& voice = voices[voiceIndex];
            if (!voice.env.isActive()) {
                voice.env.reset();
            }
        }


    protectYourEars(outputBufferLeft,sampleCount);
    protectYourEars(outputBufferRight,sampleCount);
}


void Synth::midiMessages(uint8_t data0, uint8_t data1, uint8_t data2)
/*
 * Handles incoming MIDI messages.
 *
 * @param data0 The first byte of the MIDI message (status byte).
 * @param data1 The second byte of the MIDI message (data byte 1).
 * @param data2 The third byte of the MIDI message (data byte 2).
 */
{
    // Determine the type of MIDI message based on the status byte
    switch (data0 & 0xF0) {
        // Note off message (0x80-0x8F)
        case 0x80:
            noteOff(data1 & 0x7F); ///< Turn off the note
            break;

        // Note on message (0x90-0x9F)
        case 0x90: 
        {
            uint8_t note = data1 & 0x7F; // Extract the note number
            uint8_t velo = data2 & 0x7F; // Extract the velocity

            // If velocity is non-zero, turn on the note; otherwise, turn it off
            if (velo > 0) {
                noteOn(note, velo); // Turn on the note with velocity
            } else {
                noteOff(note); // Turn off the note
            }
            break;
        }
        // Pitch bend message
        case 0xE0:
        // Pitch bend message (0xE0-0xEF)
            pitchBend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            // magic number: 0.000014102 = log(2^(-2/8192)/12)
            // pitch bend takes values between 0.89 and 1.12
            break;
        
        case 0xB0:
        // sustain pedal message
            controlChange(data1,data2);
            break;
    }
}

void Synth::setSampleRate(float sampleRate)
{
    this->sampleRate = sampleRate;
    this->inverseSampleRate = 1.0f / sampleRate;

        for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
        {
            Voice& voice = voices[voiceIndex];
            voice.setSampleRate(sampleRate);
        }
}

int Synth::findFreeVoice() const
{
    /**
     * Finds a free voice in the synthesizer.
     *
     * This algorithm iterates through all voices and returns the index of the first voice that is not active or has finished its attack phase and is at the lowest level.
     *
     * @return The index of the free voice.
     */
    int voice = 0;
    float l = 100.0f;

    for (int i = 0; i < MAX_VOICES; ++i)
    { 
        if ((!voices[i].env.isActive() || voices[i].env.level < l) && !voices[i].env.isInAttack()) {
            l = voices[i].env.level;
            voice = i;
        }
    }
    return voice;
}

void Synth::noteOn(int note, int velocity)
/** 
 * Turns on a note on the synthesizer.
 *
 * @param note The MIDI note number to turn on.
 * @param velocity The velocity (loudness) of the note, ranging from 0 to 127.
 */
{
    int voice = 0;
    if (numVoices > 1) {
        voice = findFreeVoice();
    }
    startVoice(voice, note, velocity);
}

void Synth::startVoice(int voiceIndex, int note, int velocity)
/** 
 * Sets up a specific voice for playing.
 *
 * @param voiceIndex the index of the voice to set up.
 * @param note The MIDI note number to turn on.
 * @param velocity The velocity (loudness) of the note, ranging from 0 to 127.
 */
{
    Voice& voice = voices[voiceIndex];
    voice.note = note;

    // update panning and other parameters
    voice.update();

    // oscillator 1
    // float freq = 440.0f * std::exp2((float(note - 69) + tune + ANALOG * float(voiceIndex)) / 12.0f);
    // voice.period = sampleRate / freq;
    voice.period = calculatePeriod(voiceIndex, note);
    
    volumeTrim = 0.0008f * (3.2f - oscMix - 25.0f * noiseMix) * 1.5f;
    voice.oscillator.amplitude = volumeTrim * velocity;
    // voice.oscillator.reset();

    // oscillator 2
    voice.oscillator2.amplitude = voice.oscillator.amplitude * oscMix;
    // voice.oscillator2.reset();

    // ADSR updates
    // When note is hit, set parameters for initial attack    
    voice.env.attackMultiplier = envAttack;
    voice.env.decayMultiplier = envDecay;
    voice.env.sustainLevel = envSustain;
    voice.env.releaseMultiplier = envRelease;
    
    voice.env.attack();
}

void Synth::noteOff(int note)
/*
 * Turns off a note on the synthesizer.
 *
 * @param note The MIDI note number to turn off.
 */
{
    for (int voice = 0; voice < numVoices; ++voice) 
    {
        if (voices[voice].note == note && sustainPedalPressed)
        {
            voices[voice].note = SUSTAIN;
        } else if (voices[voice].note == note) 
        {
            voices[voice].noteOff();
            voices[voice].note = 0;
        }
    }
}

void Synth::controlChange(uint8_t data1, uint8_t data2)
{
    switch (data1) 
    {
        case 0x40:
            sustainPedalPressed = (data2 >= 64);

            if (!sustainPedalPressed)
            {
                noteOff(SUSTAIN);
            }
             
            break;

        default:
            // reset all voices and sustain pedal when the PANIC! message is received
            if (data1 >= 0x78) {
                for (int voice = 0; voice < numVoices; ++voice) {
                    voices[voice].reset();
                }
                sustainPedalPressed = false;
            }
            break;
    }

}

float Synth::calculatePeriod(int voiceIndex, int note) const
{
/**
 * Calculates the period of a note based on its frequency.
 *
 * @param note The MIDI note number.
 * @return The period of the note in samples.
 */

// another magic number, this one is equal to log(2^-1/12)
// This causes a loop if period = 0.
    float period = tune * std::exp(-0.05776226505f * float(note) + ANALOG * float(voiceIndex));
// Ensure the period is 6 samples or greater, other wise the BLIT is unstable
    while (period < 6.0f || (period * detune) < 6.0f) {period += period; }
    return period;
}

float Synth::calculateAttackFromPercentage(float attackPercentage)
{
   float envAttack = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * attackPercentage));
   return envAttack;
}

float Synth::calculateDecayFromPercentage(float decayPercentage)
{
    float envDecay = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * decayPercentage));
    return envDecay;
}

float Synth::calculateSustainFromPercentage(float sustainPercentage)
{
    float envSustain = sustainPercentage / 100.0f;
    return envSustain;
}

float Synth::calculateReleaseFromPercentage(float releasePercentage)
{   
    float envRelease = 0.0f;
    if (releasePercentage < 1.0f) {
        envRelease = 0.75f; // extra fast release
    } else {
        envRelease = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * releasePercentage));
    }

    return envRelease;
}