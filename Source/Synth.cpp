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
    voice.reset();
    noise.reset();
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];

    // Loop through samples
    for (int sample = 0; sample < sampleCount; ++ sample) {
        // get next noise sample
        auto noiseSample = noise.nextValue() * noiseMix;

        // make sure note is being played, then apply velocity
        float outputSample = 0.0f;
        if (voice.env.isActive()) {
            outputSample = voice.render(noiseSample);
        }
         // copy output to each channel, only applying to left if we're in mono
        outputBufferLeft[sample] = outputSample;
        if (outputBufferRight != nullptr) {
            outputBufferRight[sample] = outputSample;
        }
    }

    if (!voice.env.isActive()) {
        voice.env.reset();
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
        case 0x90: {
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
    }
}

void Synth::noteOn(int note,int velocity)
/*
 * Turns on a note on the synthesizer.
 *
 * @param note The MIDI note number to turn on.
 * @param velocity The velocity (loudness) of the note, ranging from 0 to 127.
 */
{
    voice.note = note;
    voice.oscillator.amplitude = (velocity / 127.0f) * 0.5f;
    float frequency = 440.0f * std::exp2(float(note - 69) / 12.0f);
    voice.oscillator.period = sampleRate / frequency;
    voice.oscillator.reset();

    // ADSR updates
    // When note is hit, set parameters for initial attack
    Envelope& env = voice.env;
    env.attackMultiplier = env.attack;
    env.decayMultiplier = env.decay;
    env.sustainLevel = env.sustain;
    env.releaseMultiplier = env.release;
    env.attack();
    
}

void Synth::noteOff(int note)
/*
 * Turns off a note on the synthesizer.
 *
 * @param note The MIDI note number to turn off.
 */
{
    if (voice.note == note) {
        voice.release();
    }
}