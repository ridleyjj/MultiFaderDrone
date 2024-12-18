/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "Components/Audio/jr_Oscillators.h"
#include "Components/Audio/jr_FaderPairs.h"
#include "Components/Audio/ApvtsListener.h"

// parameter IDs
namespace ID
{
    const juce::Identifier GAIN{ "gain" };
    const juce::Identifier NUM_VOICES{ "numVoices" };
    const juce::Identifier RATE{ "rate" };
    const juce::Identifier LOCK_RANGE{ "lockRange" };
    const juce::Identifier STEREO_WIDTH{ "stereoWidth" };
    const juce::Identifier FREQ_RANGE_MIN{ "freqRangeMin" };
    const juce::Identifier FREQ_RANGE_MAX{ "freqRangeMax" };
    const juce::Identifier DARK_MODE{ "darkMode" };
    const juce::Identifier WAVE_SHAPE{ "waveShape" };
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

    void setNumOscs(int _numOscs) { faders.setNumOscs(_numOscs); }
    
    void setLfoRate(float _rate) { faders.setLfoRate(jr::Utils::constrainFloat(_rate)); }

    void setMinOscFreq(float minHz) { faders.setMinFreq(minHz); }

    void setMaxOscFreq(float maxHz) { faders.setMaxFreq(maxHz); }

    /*
    * Sets stereo width to a value between 0 - 1.0 where 1.0 is full stereo width and 0 is mono
    */
    void setStereoWidth(float width) { faders.setStereoWidth(jr::Utils::constrainFloat(width)); }

    void setGain(double _gain) { gain.setTargetValue(jr::Utils::constrainFloat(_gain) * maxGain); }

    void setWaveShape(float _waveShape) { faders.setWaveShape(_waveShape); }

    float getMaxFreq() { return maxFreq; }

    float getMinFreq() { return minFreq; }
    
    float getDefaultMinFreq() { return defaultMinFreq; }
    
    float getDefaultMaxFreq() { return defaultMaxFreq; }

    float getGain() { return gain.getCurrentValue() * (1.0f / maxGain); }

    bool getRangeLocked() {
        return (bool)(*apvts.getRawParameterValue(ID::LOCK_RANGE.toString()));
    }

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    std::shared_ptr<std::vector<FaderPairs::RandomOsc>> getOscs() { return faders.getOscs(); }

private:
    float maxGain = 0.75;
    FaderPairs faders;              // class containing all RandomOscs controlled by their own random faders
    int maxOscCount{ 100 };
    float maxFreq{ 2000.0f };       // max freq in Hz that Osc Freq slider can be set
    float minFreq{ 60.0f };         // min freq in Hz that Osc Freq slider can be set
    float defaultMinFreq{ 120.0f };
    float defaultMaxFreq{ 1200.0f };

    // GUI Params
    juce::SmoothedValue<float> gain{ maxGain };               // master output level

    juce::AudioProcessorValueTreeState apvts;

    jr::ApvtsListener gainListener{ [&](float newValue) { setGain(newValue); } };
    jr::ApvtsListener rateListener{ [&](float newValue) { setLfoRate(newValue); } };
    jr::ApvtsListener voicesListener{ [&](float newValue) { setNumOscs(newValue); } };
    jr::ApvtsListener stereoWidthListener{ [&](float newValue) { setStereoWidth(newValue); } };
    jr::ApvtsListener minFreqRangeListener{ [&](float newValue) { setMinOscFreq(newValue); } };
    jr::ApvtsListener maxFreqRangeListener{ [&](float newValue) { setMaxOscFreq(newValue); } };
    jr::ApvtsListener waveShapeListener{ [&](float newValue) { setWaveShape(newValue); } };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiFaderDroneAudioProcessor)
};
