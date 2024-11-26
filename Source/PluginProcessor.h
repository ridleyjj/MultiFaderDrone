/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"
#include "jr_FaderPairs.h"
#include "ApvtsListener.h"

// parameter IDs
namespace ID
{
    const juce::Identifier GAIN{ "gain" };
    const juce::Identifier NUM_VOICES{ "numVoices" };
    const juce::Identifier RATE{ "rate" };
    const juce::Identifier FREEZE_RANGE{ "freeze" };
}

//==============================================================================
/**
*/
class MultiFaderDroneAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MultiFaderDroneAudioProcessor();
    ~MultiFaderDroneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

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

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void setNumPairs(int _oscCount);
    
    void setLfoRate(float _rate);

    void setOscFreqRange(float minHz, float maxHz);

    /*
    * Sets stereo width to a value between 0 - 1.0 where 1.0 is full stereo width and 0 is mono
    */
    void setStereoWidth(float width);

    void setGain(double _gain);

    void setRangeFrozen(bool isFrozen);

    void setPrevRangeMin(double newValue) { prevMinFreq = newValue; }
    
    void setPrevRangeMax(double newValue) { prevMaxFreq = newValue; }

    float getMaxFreq() { return maxFreq; }

    float getMinFreq() { return minFreq; }
    
    float getDefaultMinFreq() { return defaultMinFreq; }
    
    float getDefaultMaxFreq() { return defaultMaxFreq; }


    float getCurrentFreqRangeMin() { return currentFreqRangeMin; }

    float getCurrentFreqRangeMax() { return currentFreqRangeMax; }

    float getStereoWidth() { return stereoWidth; }

    float getGain() { return gain.getCurrentValue() * (1.0f / maxGain); }

    bool getRangeFrozen() {
        return (bool)(*apvts.getRawParameterValue(ID::FREEZE_RANGE.toString()));
    }

    double getFrozenRangeAmount() { return frozenRangeAmount; }

    double getPrevMinRange() { return prevMinFreq; }

    double getPrevMaxRange() { return prevMaxFreq; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    std::shared_ptr<std::vector<FaderPair>> getPairs() { return faders.getPairs(); }

private:
    float maxGain = 0.3;
    FaderPairs faders;              // pair of connected faders
    int maxPairCount{ 15 };
    float maxFreq{ 2400.0f };       // max freq in Hz that Osc Freq slider can be set
    float minFreq{ 80.0f };         // min freq in Hz that Osc Freq slider can be set
    float defaultMinFreq{ 120.0f };
    float defaultMaxFreq{ 1200.0f };

    // GUI Params
    juce::SmoothedValue<float> gain{ maxGain };               // master output level
    float currentFreqRangeMin{ defaultMinFreq };
    float currentFreqRangeMax{ defaultMaxFreq };
    float stereoWidth{ 0.0f };
    double frozenRangeAmount{ currentFreqRangeMax - currentFreqRangeMin };      // "Frozen" difference between Max and Min Frequency in Hz
    double prevMinFreq{};            // prev Min Freq value to determine which frequency slider head has moved
    double prevMaxFreq{};            // prev Max Freq value to determine which frequency slider head has moved

    juce::AudioProcessorValueTreeState apvts;

    jr::ApvtsListener gainListener{ [&](float newValue) { setGain(newValue); } };
    jr::ApvtsListener rateListener{ [&](float newValue) { setLfoRate(newValue); } };
    jr::ApvtsListener voicesListener{ [&](float newValue) { setNumPairs(newValue); } };
    jr::ApvtsListener freezeListener{ [&](bool newValue) { setRangeFrozen(newValue); } };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiFaderDroneAudioProcessor)
};
