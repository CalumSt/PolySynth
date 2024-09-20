#pragma once
#include "Synth.h"
#include "Utils.h"



// This class defines the Synthesiser interface and rendering methods.
// 15/07/2024

Synth::Synth()
{
    sampleRate = 44100.0f;
}

void Synth::allocateResources(double sampleRate_,int /*samplesPerBlock*/)
{
    sampleRate = static_cast<float>(sampleRate_);

    for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
    {
        voices[voiceIndex].filter.setSampleRate(sampleRate);
    }
}

void Synth::deallocateResources() const
{
    // Currently not implemented. It will be once the components are together.
}

void Synth::reset()
{
    lfo = 0.0f;
    lfoStep = 0;

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
        // update the LFO
        updateLFO();

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
    // TODO: Switch these to std::byte for byte-oriented data manipulation
    switch (data0 & 0xF0) {
        // Note off message (0x80-0x8F)
        case 0x80:
            noteOff(data1 & 0x7F); ///< Turn off the note
            break;

        // Note on message (0x90-0x9F)
        case 0x90: 
        {
            const uint8_t note = data1 & 0x7F; // Extract the note number

            // If velocity is non-zero, turn on the note; otherwise, turn it off
            // Extract the velocity is done implicitly
            if (uint8_t velo = data2 & 0x7F; velo > 0) {
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

        default:
            break;
    }
}

void Synth::updateLFO() // TODO: Comment me!
{
    if (--lfoStep <= 0 )
    {
        lfoStep = LFO_MAX;

        lfo += lfoInc;
        if (lfo > PI) { lfo -= TWO_PI; }
        const float sine = std::sin(lfo);
        // TODO: Remove hardcoding!
        float vibratoMod = 1.0f + sine * 0.1f;

        for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
        {
            Voice& voice = voices[voiceIndex];
            voice.filter.updateCoefficients (1000.0f,0.707f);
            if (voice.env.isActive())
            {
                voice.oscillator.modulation = vibratoMod;
                voice.oscillator2.modulation = vibratoMod;
            }
        }
    }
}


void Synth::setSampleRate(float inputSampleRate)
{
    this->sampleRate = inputSampleRate;
    this->inverseSampleRate = 1.0f / inputSampleRate;

        for (int voiceIndex = 0; voiceIndex < MAX_VOICES; ++voiceIndex)
        {
            Voice& voice = voices[voiceIndex];
            voice.setSampleRate(inputSampleRate);
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
    if (ignoreVelocity) { velocity = 80; }
    
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

    voice.period = calculatePeriod(voiceIndex, note);

    // Automatic Gain Control and Velocity
    volumeTrim = 0.0008f * (3.2f - oscMix - 25.0f * noiseMix) * 1.5f;
    float mappedVelocity = 0.004f *float((velocity + 64) * (velocity +64)) - 8.0f;
    voice.oscillator.amplitude = volumeTrim * mappedVelocity;

    // oscillator 2
    voice.oscillator2.amplitude = voice.oscillator.amplitude * oscMix;

    // ADSR updates
    // When note is hit, set parameters for initial attack    
    voice.env.attackMultiplier = envAttack;
    voice.env.decayMultiplier = envDecay;
    voice.env.sustainLevel = envSustain;
    voice.env.releaseMultiplier = envRelease;
    
    voice.env.attack();
}

// declare unused for now, will come back to this
void Synth::restartMonoVoice (const int note, [[maybe_unused]] int velocity)
{
    const float period = calculatePeriod(0, note);

    Voice& voice =voices [0];
    voice.period = period;
    voice.env.level += SILENCE + SILENCE;
    voice.note = note;
    voice.update();
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
    // moved from switch to if for now
    if (data1 == 0x40)
    {
        sustainPedalPressed = (data2 >= 64);

        if (!sustainPedalPressed)
        {
            noteOff(SUSTAIN);
        }
    } else {
        // reset all voices and sustain pedal when the PANIC! message is received
        if (data1 >= 0x78) {
            for (int voice = 0; voice < numVoices; ++voice) {
                voices[voice].reset();
            }
            sustainPedalPressed = false;
            }
    }

}

float Synth::calculatePeriod (const int voiceIndex, const int note) const
{
/**
 * Calculates the period of a note based on its frequency.
 *
 * @param note The MIDI note number.
 * @return The period of the note in samples.
 */

// another magic number, this one is equal to log(2^-1/12)
// This causes a loop if period = 0.
    float period = tune * std::exp(-0.05776226505f * static_cast<float> (note) + ANALOG * float(voiceIndex));
// Ensure the period is 6 samples or greater, otherwise the BLIT is unstable
    while (period < 6.0f || (period * detune) < 6.0f) {period += period; }
    return period;
}

float Synth::calculateAttackFromPercentage (const float attackPercentage) const
{
    const float calculatedEnvAttack = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * attackPercentage));
   return calculatedEnvAttack;
}

float Synth::calculateDecayFromPercentage (const float decayPercentage) const
{
    float calculatedEnvDecay = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * decayPercentage));
    return calculatedEnvDecay;
}

float Synth::calculateSustainFromPercentage (const float sustainPercentage) const
{
    float calculatedEnvSustain = sustainPercentage / 100.0f;
    return calculatedEnvSustain;
}

float Synth::calculateReleaseFromPercentage (const float releasePercentage) const
{   
    float calculatedEnvRelease = 0.0f;
    if (releasePercentage < 1.0f) {
        calculatedEnvRelease = 0.75f; // extra fast release
    } else {
        calculatedEnvRelease = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * releasePercentage));
    }

    return calculatedEnvRelease;
}