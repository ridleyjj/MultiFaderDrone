/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StyleSheet.h"
#include <memory>
#include "OscillatorVisualiser.h"

//==============================================================================
/**
*/
class MultiFaderDroneAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Timer
{
public:
    MultiFaderDroneAudioProcessorEditor (MultiFaderDroneAudioProcessor&);
    ~MultiFaderDroneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

    void timerCallback() override;

    void freqRangeSliderUpdate();

    void stereoSliderUpdate();

    void toggleRangeFrozen();

private:
    // helpers

    void initSimpleSlider(juce::Slider* slider, juce::Label* label, const juce::String& name);
    
    void initSimpleSliderWithRange(juce::Slider* slider, juce::Label* label, const juce::String& name, double minVal, double maxValue, double step);

    CustomLookAndFeel myLookAndFeel;

    // sliders and labels

    juce::Slider voicesSlider{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider gainSlider{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider lfoRateSlider{ juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow };
    juce::Slider stereoSlider{ juce::Slider::SliderStyle::TwoValueHorizontal, juce::Slider::NoTextBox };
    juce::Slider freqRangeSlider{ juce::Slider::SliderStyle::TwoValueVertical, juce::Slider::TextBoxBelow };
    juce::Label voicesLabel, lfoRateLabel, freqRangeLabel, gainLabel, stereoLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, lfoRateAttachment, voicesAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttachment;

    // stereo width state

    double prevStereoMin = 0.0;
    double prevStereoMax = 0.0;

    // buttons

    juce::ToggleButton freezeRangeButton{ "Freeze" };

    // Visualiser

    jr::OscillatorVisualiser visualiser{};

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultiFaderDroneAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiFaderDroneAudioProcessorEditor)
};
