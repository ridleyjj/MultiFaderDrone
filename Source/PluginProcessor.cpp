/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <memory>
#include <functional>

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
                       ), apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    apvts.addParameterListener(ID::GAIN.toString(), &gainListener);
    apvts.addParameterListener(ID::RATE.toString(), &rateListener);
    apvts.addParameterListener(ID::NUM_VOICES.toString(), &voicesListener);
}

MultiFaderDroneAudioProcessor::~MultiFaderDroneAudioProcessor()
{
    apvts.removeParameterListener(ID::GAIN.toString(), &gainListener);
    apvts.removeParameterListener(ID::RATE.toString(), &rateListener);
    apvts.removeParameterListener(ID::NUM_VOICES.toString(), &voicesListener);
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
    int currentNumVoices = floor(*apvts.getRawParameterValue(ID::NUM_VOICES.toString()));
    faders.init(currentNumVoices, sampleRate, maxPairCount);
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
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MultiFaderDroneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiFaderDroneAudioProcessor();
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout MultiFaderDroneAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::GAIN.toString(), "Gain", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::RATE.toString(), "Rate", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(ID::NUM_VOICES.toString(), "Num Voices", 1, 15, 3, "Num Voices", [](int value, int maximumStringLength) -> juce::String { return juce::String(value * 2); }, nullptr));

    return layout;
}

// update method calls from Editor

void MultiFaderDroneAudioProcessor::setNumPairs(int _numPairs)
{
    faders.setNumPairs(_numPairs);
}

void MultiFaderDroneAudioProcessor::setLfoRate(float _rate)
{
    faders.setLfoRate(jr::Utils::constrainFloat(_rate));
}

void MultiFaderDroneAudioProcessor::setOscFreqRange(float minHz, float maxHz)
{
    currentFreqRangeMin = jr::Utils::constrainFloat(minHz, minFreq, maxFreq);
    currentFreqRangeMax = jr::Utils::constrainFloat(maxHz, minFreq, maxFreq);
    faders.setOscFreqRange(currentFreqRangeMin, currentFreqRangeMax);
}

void MultiFaderDroneAudioProcessor::setGain(double _gain)
{
    gain.setTargetValue(jr::Utils::constrainFloat(_gain) * maxGain);
}

void MultiFaderDroneAudioProcessor::setStereoWidth(float width)
{
    stereoWidth = jr::Utils::constrainFloat(width);
    faders.setStereoWidth(stereoWidth);
}