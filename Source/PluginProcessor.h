/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include "Synth.h"
#include "Utils.h"
#include <JuceHeader.h>
#include <foleys_gui_magic/General/foleys_MagicProcessor.h>
//==============================================================================
class JX11AudioProcessor : public foleys::MagicProcessor, juce::ValueTree::Listener
{
public:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameterTree { *this, nullptr, "Parameters", createParameterLayout() };

    //==============================================================================
    JX11AudioProcessor();
    ~JX11AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    // Synth Parameters
    juce::AudioParameterFloat* oscMix;
    juce::AudioParameterFloat* oscTune;
    juce::AudioParameterFloat* oscFine;
    juce::AudioParameterChoice* glideMode;
    juce::AudioParameterFloat* glideRate;
    juce::AudioParameterFloat* glideBend;
    juce::AudioParameterFloat* filterFreq;
    juce::AudioParameterFloat* filterReso;
    juce::AudioParameterFloat* filterLfo;
    juce::AudioParameterFloat* filterEnv;
    juce::AudioParameterFloat* filterVelocity;
    juce::AudioParameterFloat* filterAttack;
    juce::AudioParameterFloat* filterDecay;
    juce::AudioParameterFloat* filterSustain;
    juce::AudioParameterFloat* filterRelease;
    juce::AudioParameterFloat* envelopeAttack;
    juce::AudioParameterFloat* envelopeDecay;
    juce::AudioParameterFloat* envelopeSustain;
    juce::AudioParameterFloat* envelopeRelease;
    juce::AudioParameterFloat* lfoRate;
    juce::AudioParameterFloat* vibrato;
    juce::AudioParameterFloat* noise;
    juce::AudioParameterFloat* octave;
    juce::AudioParameterFloat* tuning;
    juce::AudioParameterFloat* outputLevel;
    juce::AudioParameterChoice* polyMode;
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    //==============================================================================

    void splitBufferByEvents(juce::AudioBuffer<float>&buffer, juce::MidiBuffer& midiMessages);
    void handleMidi(uint8_t data0, uint8_t data1, uint8_t data2);
    void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);

    Synth synth;
    //==============================================================================
    private:
    std::atomic<bool> parametersChanged { false }; // Use an atomic bool to check for any parameter changes
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
      parametersChanged.store(true);
    }
    void update();

    // GUI MAGIC: define that as last member of your AudioProcessor
    foleys::MagicLevelSource* outputMeter = nullptr;
    foleys::MagicPlotSource* oscilloscope = nullptr;
    foleys::MagicPlotSource* analyser = nullptr;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JX11AudioProcessor)
};
