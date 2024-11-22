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

    void setOscCount(size_t _oscCount);
    
    void setLfoRate(float _rate);

    void setOscFreqRange(float minHz, float maxHz);

    /*
    * Sets stereo width to a value between 0 - 1.0 where 1.0 is full stereo width and 0 is mono
    */
    void setStereoWidth(float width);

    void setGain(double _gain);

    void setRangeFrozen(bool isFrozen) { rangeFrozen = isFrozen; }

    size_t getMaxOscCount() {
        return maxOscCount;
    }

    float getMaxFreq() { return maxFreq; }

    float getMinFreq() { return minFreq; }
    
    float getDefaultMinFreq() { return defaultMinFreq; }
    
    float getDefaultMaxFreq() { return defaultMaxFreq; }

    int getNumPairs() { return numPairs; }

    float getCurrentFreqRangeMin() { return currentFreqRangeMin; }

    float getCurrentFreqRangeMax() { return currentFreqRangeMax; }

    float getRate() { return rate; }

    float getStereoWidth() { return stereoWidth; }

    float getGain() { return gain.getCurrentValue() * (1.0f / maxGain); }

    bool getRangeFrozen() { return rangeFrozen; }

private:
    size_t oscCount{ 2 };          // total number of oscillators
    float maxGain = 0.3;
    FaderPairs faders;              // pair of connected faders
    size_t maxOscCount{ 30 };
    float maxFreq{ 2400.0f };       // max freq in Hz that Osc Freq slider can be set
    float minFreq{ 80.0f };         // min freq in Hz that Osc Freq slider can be set
    float defaultMinFreq{ 120.0f };
    float defaultMaxFreq{ 1200.0f };

    // GUI Params
    juce::SmoothedValue<float> gain{ maxGain };               // master output level
    int numPairs{ 1 };
    float currentFreqRangeMin{ 120.0f };
    float currentFreqRangeMax{ 1200.0f };
    float rate{ 0.0f };
    float stereoWidth{ 0.0f };
    bool rangeFrozen{ false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiFaderDroneAudioProcessor)
};
