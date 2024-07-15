#include "Synth.h"

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
        auto noiseSample = noise.nextValue();

        // make sure note is being played, then apply velocity
        float output = 0.0f;
        if (voice.note > 0) {
            output = noiseSample * (voice.velocity / 127.0f) * 0.5f;
        }
         // copy output to each channel, only applying to left if we're in mono
        outputBufferLeft[sample] = output;
        if (outputBufferRight != nullptr) {
            outputBufferRight[sample] = output;
        }
    }
}

void Synth::midiMessages(uint8_t data0,uint8_t data1,uint8_t data2)
{
    switch (data0 & 0xF0) {
         // Note off
         case 0x80:
            noteOff(data1 & 0x7F);
            break;
        
        case 0x90: {
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note, velo);
            } else {
                noteOff(note);
            }
            break;
        }
    }
}

void Synth::noteOn(int note,int velocity)
{
    voice.note;
    voice.velocity = velocity;
}

void Synth::noteOff(int note)
{
   if (voice.note == note) {
    voice.note = 0;
    voice.velocity = 0;
   }
}