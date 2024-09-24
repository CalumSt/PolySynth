/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cstdint>

//==============================================================================
JX11AudioProcessor::JX11AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    parameterTree.state.addListener(this);
}

JX11AudioProcessor::~JX11AudioProcessor()
{
    parameterTree.state.removeListener(this);
}

//==============================================================================
const juce::String JX11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JX11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JX11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JX11AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JX11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void JX11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JX11AudioProcessor::getProgramName (int index)
{
    return {};
}

void JX11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JX11AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.allocateResources(sampleRate, samplesPerBlock);
    parametersChanged.store(true);
    synth.reset();
}

void JX11AudioProcessor::releaseResources()
{
    synth.deallocateResources();
}

void JX11AudioProcessor::reset()
{
    synth.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JX11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only{}
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JX11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessageList)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (bool expected = true; isNonRealtime() || parametersChanged.compare_exchange_strong(expected,false)) {
        update(); // This function is used to update parameters
    }

    // Process MIDI events - render is held in this too
    splitBufferByEvents(buffer, midiMessageList);
}

//==============================================================================
bool JX11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JX11AudioProcessor::createEditor()
{
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500,1050);
    return editor;
}

//==============================================================================
void JX11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JX11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout JX11AudioProcessor::createParameterLayout() {

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    // Lambda functions
    auto oscMixStringFromValue = [](float value, int)
    {
        char s[16] = { 0 };
        snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value, 0.5f * value);
        return juce::String(s);
    };

    auto filterVelocityStringFromValue = [](float value, int)
    {
        if (value > -90.0f)
            return juce::String("OFF");
        else
            return juce::String(value);
    };

    auto lfoRateStringFromValue = [](float value, int)
    {
        float lfoHz = std::exp(7.0f * value - 4.0f);
        return juce::String(lfoHz,3);
    };

    auto vibratoStringFromValue = [](float value, int)
    {
        if (value > 0.0f)
            return "PWM " + juce::String(-value,1);
        else
            return juce::String(value,1);
    };

    // Parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>("polyMode", "Polyphony",
               juce::StringArray{"Mono","Poly"},1));

    layout.add(std::make_unique<juce::AudioParameterFloat>("oscTune", "Osc Tune",
               juce::NormalisableRange<float>(-24.0f,24.0f,1.0f),-12.0f,
               juce::AudioParameterFloatAttributes().withLabel("semi")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("oscFine", "Osc Fine",
               juce::NormalisableRange<float>(-50.0f,50.0f,0.1f,0.3f,true),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("cent")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("oscMix", "Osc Mix",
               juce::NormalisableRange<float>(0.0f,100.0f),0.0f,
               juce::AudioParameterFloatAttributes()
               .withLabel("%")
               .withStringFromValueFunction(oscMixStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterChoice>("glideMode", "Glide Mode",
               juce::StringArray{"Off", "Legato","Always"},0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("glideRate", "Glide Rate",
               juce::NormalisableRange<float>(0.0f,100.f,1.0f),35.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("glideBend", "Glide Bend",
               juce::NormalisableRange<float>(-36.0f,36.0f,0.01f,0.4f,true),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("Semi")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterFreq", "Filter Freq.",
               juce::NormalisableRange<float>(0.0f,100.0f,0.1f),100.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterReso", "Filter Reso.",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),15.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterEnv", "Filter Env.",
               juce::NormalisableRange<float>(-100.0f,100.0f,0.1f),50.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterLFO", "Filter LFO",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterVelocity", "Filter Vel.",
               juce::NormalisableRange<float>(-100.0f,100.0f,1.0f),0.0f,
               juce::AudioParameterFloatAttributes()
               .withLabel("%")
               .withStringFromValueFunction(filterVelocityStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterAttack", "Filter Atk.",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterDecay", "Filter Dec.",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),30.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterSustain", "Filter Sus.",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("filterRelease", "Filter Rel.",
               juce::NormalisableRange<float>(30.0f,30000.0f,0.01f,0.25f),1500.0f,
               juce::AudioParameterFloatAttributes().withLabel("ms")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("envAttack", "Env. Attack",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("envDecay", "Env. Decay",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),50.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("envSustain", "Env. Sustain",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),100.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("envRelease", "Env. Release",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),30.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Rate",
               juce::NormalisableRange<float>(),0.81f,
               juce::AudioParameterFloatAttributes()
               .withLabel("%")
               .withStringFromValueFunction(lfoRateStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>("vibrato", "Vibrato",
               juce::NormalisableRange<float>(-100.0f,100.0f,0.1f),0.0f,
               juce::AudioParameterFloatAttributes()
               .withLabel("%")
               .withStringFromValueFunction(vibratoStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>("noise", "Noise",
               juce::NormalisableRange<float>(0.0f,100.0f,1.0f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("octave", "Octave",
               juce::NormalisableRange<float>(-2.0f,2.0f,1.0f),0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("tuning", "Tuning",
               juce::NormalisableRange<float>(-100.0f,100.0f,0.1f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("cent")));

    layout.add(std::make_unique<juce::AudioParameterFloat>("outputLevel", "Output Level",
               juce::NormalisableRange<float>(-24.0f,6.0f,0.1f),0.0f,
               juce::AudioParameterFloatAttributes().withLabel("dB")));

    return layout;
}

//==============================================================================
void JX11AudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessageList)
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
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    // TODO: remove raw pointers and replace with JuceAudioBuffer
    synth.render(outputBuffers, sampleCount);
}

void JX11AudioProcessor::update()
{
    // This method interfaces changes to the parameter tree to the synth engine
    auto sampleRate = float(getSampleRate());

    synth.setSampleRate(sampleRate);
    synth.envAttack = synth.calculateAttackFromPercentage(parameterTree.getRawParameterValue("envAttack")->load());
    synth.envDecay = synth.calculateDecayFromPercentage(parameterTree.getRawParameterValue("envDecay")->load());
    synth.envSustain = synth.calculateSustainFromPercentage(parameterTree.getRawParameterValue("envSustain")->load());
    synth.envRelease = synth.calculateReleaseFromPercentage(parameterTree.getRawParameterValue("envRelease")->load());
    // Filter envelope
    synth.filterAttack = synth.calculateAttackFromPercentage (parameterTree.getRawParameterValue ("filterAttack")->load());
    synth.filterDecay = synth.calculateDecayFromPercentage (parameterTree.getRawParameterValue ("filterDecay")->load());
    synth.filterSustain = synth.calculateSustainFromPercentage (parameterTree.getRawParameterValue ("filterSustain")->load());
    synth.filterRelease = synth.calculateReleaseFromPercentage (parameterTree.getRawParameterValue ("filterRelease")->load());
    synth.filterEnvDepth = 0.06f * parameterTree.getRawParameterValue ("filterEnv")->load();

    // Oscillators
    synth.oscMix = parameterTree.getRawParameterValue("oscMix")->load() / 100.0f;
    float semi = parameterTree.getRawParameterValue("oscTune")->load();
    float cent = parameterTree.getRawParameterValue("oscFine")->load();
    synth.detune = std::pow(1.059463094359f, -semi - 0.01f * cent);
    // This is equivalent to std::exp2((-semi - 0.01f * cent) / 12.0f)

    // Synth tuning
    float octave = parameterTree.getRawParameterValue("octave")->load();
    float tuning = parameterTree.getRawParameterValue("tuning")->load();

    float tuneInSemi = -36.3763f - 12.0f * octave - tuning / 100.0f;
    synth.tune = sampleRate * std::exp(0.05776226505f * tuneInSemi);
    
    // Poly/Mono
    auto polyMode = parameterTree.getRawParameterValue("polyMode")->load();
    synth.numVoices = (polyMode == 0) ? 1 : Synth::MAX_VOICES;

    // Lfo parameters
    float lfoRate = parameterTree.getRawParameterValue("lfoRate")->load();
    synth.lfo.setLfoRate (lfoRate);

    // get the pointer to the atomic and load it, then scale it
    float noise = parameterTree.getRawParameterValue ("noise")->load() / 100.0f;
    // Save to Synth object
    noise *= noise;
    synth.noiseMix = noise * 0.06f;

    // these are throwing a non-fatal error
    float filterResonance = parameterTree.getRawParameterValue ("filterReso")->load();
    synth.filterResonance = filterResonance * 0.01f;

    // filter key tracking
    synth.filterKeyTracking = 0.08f * parameterTree.getRawParameterValue ("filterFreq")->load() - 1.5f;

    // filter LFO depth
    float filterLfoDepth = 0.01f * parameterTree.getRawParameterValue ("filterLFO")->load();
    synth.filterLfoDepth = 2.5f * filterLfoDepth * filterLfoDepth;

    // vibrato
    float vibrato = parameterTree.getRawParameterValue ("vibrato")->load() / 200.0f;
    synth.vibrato = 0.2f * vibrato * vibrato;

    synth.volumeTrim = 0.0008f * (3.2f - synth.oscMix - 25.0f * synth.noiseMix) * 1.5f;
    // This formula comes from the JX10, and why it was chosen is unknown, but it works for automatic gain control.
    // I may want to move this to the synth engine

    synth.outputLevel = juce::Decibels::decibelsToGain(parameterTree.getRawParameterValue("outputLevel")->load());

    float filterVelocity = parameterTree.getRawParameterValue("filterVelocity")->load();
    if (filterVelocity < -90.0f) {
        synth.velocitySensitivity = 0.0f;
        synth.ignoreVelocity = true;
    } else {
        synth.velocitySensitivity = 0.0005f * filterVelocity;
        synth.ignoreVelocity = false;
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11AudioProcessor();
}
