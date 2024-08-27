/*****************************************************************************
  ,ad8888ba,    88        88  88  88      888888888888  ad88888ba   
 d8"'    `"8b   88        88  88  88           88      d8"     "8b  
d8'        `8b  88        88  88  88           88      Y8,          
88          88  88        88  88  88           88      `Y8aaaaa,    
88          88  88        88  88  88           88        `"""""8b,  
Y8,    "88,,8P  88        88  88  88           88              `8b  
 Y8a.    Y88P   Y8a.    .a8P  88  88           88      Y8a     a8P  
  `"Y8888Y"Y8a   `"Y8888Y"'   88  88888888888  88       "Y88888P"   


 * @file JuceHandler.h
 * @author CS Islay
 * @brief Utility class to handle JUCE-specific things, 
 *        and outputting non-framework-specific outputs to the Synth engine.
 * 
 ****************************************************************************/

# pragma once

#include <JuceHeader.h>
#include "Synth.h"

class JuceHandler
{
void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessageList)
{
    int bufferOffset = 0;

    for (const auto midiMessage : midiMessageList) {
        int samplesThisSegment = midiMessage.samplePosition - bufferOffset;
        // Render the audio that happened before this event if any
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment,bufferOffset);
            bufferOffset += samplesThisSegment;
        }

        if (midiMessage.numBytes <= 3) {
            uint8_t data1 = (midiMessage.numBytes >= 2) ? midiMessage.data[1] : 0;
            uint8_t data2 = (midiMessage.numBytes == 3) ? midiMessage.data[2] : 0;
            handleMidi(midiMessage.data[0],data1,data2);
        }
    }
    // Render audio after the last midi event
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment,bufferOffset);
    }

    midiMessageList.clear();
    
}
void JX11AudioProcessor::handleMidi(uint8_t data0, uint8_t data1, uint8_t data2)
{
    synth.midiMessages(data0, data1, data2);
}

void JX11AudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    float* outputBuffers[2] = { nullptr, nullptr };
    // Write first channel
    // add offset to Pointer
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    // If stereo, write to 2nd channel
    if (getTotalNumInputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    // TODO: remove raw pointers and replace with JuceAudioBuffer
    synth.render(outputBuffers, sampleCount);
}
}
