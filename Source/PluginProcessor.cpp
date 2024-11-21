/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultiFaderDroneAudioProcessor::MultiFaderDroneAudioProcessor()
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
}

MultiFaderDroneAudioProcessor::~MultiFaderDroneAudioProcessor()
{
}

//==============================================================================
const juce::String MultiFaderDroneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultiFaderDroneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultiFaderDroneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultiFaderDroneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultiFaderDroneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultiFaderDroneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultiFaderDroneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultiFaderDroneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultiFaderDroneAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultiFaderDroneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultiFaderDroneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    faders.init(oscCount / 2, sampleRate, maxOscCount / 2);
    gain.reset(sampleRate, 0.1f);
}

void MultiFaderDroneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultiFaderDroneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
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

void MultiFaderDroneAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    float numSamples = buffer.getNumSamples();

    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    //======================================== DSP LOOP ========================================
    for (int i = 0; i < numSamples; i++)
    {
        auto sampleOut = faders.process();

        gain.getNextValue();

        leftChannel[i] = sampleOut.first * gain.getCurrentValue();
        rightChannel[i] = sampleOut.second * gain.getCurrentValue();
    }
}

//==============================================================================
bool MultiFaderDroneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MultiFaderDroneAudioProcessor::createEditor()
{
    return new MultiFaderDroneAudioProcessorEditor (*this);
}

//==============================================================================
void MultiFaderDroneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MultiFaderDroneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiFaderDroneAudioProcessor();
}

//==============================================================================
// update method calls from Editor

void MultiFaderDroneAudioProcessor::setOscCount(size_t _oscCount)
{
    if (_oscCount != oscCount)
    {
        oscCount = _oscCount;
        faders.setNumPairs(oscCount / 2);
    }
}

void MultiFaderDroneAudioProcessor::setLfoRate(float _rate)
{
    faders.setLfoRate(_rate);
}

void MultiFaderDroneAudioProcessor::setOscFreqRange(float minHz, float maxHz)
{
    faders.setOscFreqRange(minHz, maxHz);
}

void MultiFaderDroneAudioProcessor::setGain(double _gain)
{
    gain.setTargetValue(jr::Utils::constrainFloat(_gain) * maxGain);
}

void MultiFaderDroneAudioProcessor::setStereoWidth(float width)
{
    faders.setStereoWidth(width);
}