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
#include "MirrorSliderAttachment.h"
#include "TwoHeadedSliderAttachment.h"
#include "LockingTwoHeadedSlider.h"

//==============================================================================
/**
*/
class MultiFaderDroneAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    MultiFaderDroneAudioProcessorEditor (MultiFaderDroneAudioProcessor&);
    ~MultiFaderDroneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

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
    jr::LockingTwoHeadedSlider freqRangeSlider{ true, juce::Slider::TextBoxBelow };
    juce::Label voicesLabel, lfoRateLabel, freqRangeLabel, gainLabel, stereoLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, lfoRateAttachment, voicesAttachment;

    std::unique_ptr<jr::MirrorSliderAttachment> stereoWidthAttachment;

    std::unique_ptr<jr::TwoHeadedSliderAttachment> freqRangeAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lockRangeAttachment;

    // buttons
    
    juce::ToggleButton lockRangeButton{ "Lock Range" };

    // Visualiser

    jr::OscillatorVisualiser visualiser{};

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultiFaderDroneAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiFaderDroneAudioProcessorEditor)
};
