/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel/StyleSheet.h"
#include <memory>
#include "Components/GUI/OscillatorVisualiser.h"
#include "Components/GUI/MirrorSliderAttachment.h"
#include "Components/GUI/TwoHeadedSliderAttachment.h"
#include "Components/GUI/LockingTwoHeadedSlider.h"
#include "Components/GUI/DarkModeButton.h"

//==============================================================================
/**
*/
class MultiFaderDroneAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer, public juce::Button::Listener
{
public:
    MultiFaderDroneAudioProcessorEditor (MultiFaderDroneAudioProcessor&);
    ~MultiFaderDroneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

    void timerCallback() override;

private:
    // helpers

    void initSimpleSlider(juce::Slider* slider, juce::Label* label, const juce::String& name);
    
    void initSimpleSliderWithRange(juce::Slider* slider, juce::Label* label, const juce::String& name, double minVal, double maxValue, double step);

    /*
    Sends updates to certain GUI objects when the look and feel is updated
    */
    void sendNewLookAndFeel();

    /*
    Method to call when dark mode has been toggled in order to refresh the new styling in the GUI
    */
    void refreshStyles();

    jr::CustomLookAndFeel myLookAndFeel;

    // sliders and labels

    juce::Slider voicesSlider{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider gainSlider{ juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider lfoRateSlider{ juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow };
    juce::Slider stereoSlider{ juce::Slider::SliderStyle::TwoValueHorizontal, juce::Slider::NoTextBox };
    juce::Slider waveShapeSlider{ juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::NoTextBox };
    jr::LockingTwoHeadedSlider freqRangeSlider{ true, juce::Slider::TextBoxBelow };
    juce::Label voicesLabel, lfoRateLabel, freqRangeLabel, gainLabel, stereoLabel, waveShapeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, lfoRateAttachment, voicesAttachment, waveShapeAttachment;

    std::unique_ptr<jr::MirrorSliderAttachment> stereoWidthAttachment;

    std::unique_ptr<jr::TwoHeadedSliderAttachment> freqRangeAttachment;

    // buttons
    
    juce::ToggleButton lockRangeButton{ "Lock Range" };
    jr::DarkModeButton darkModeButton{};

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lockRangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> darkModeAttachment;

    // Visualiser

    jr::OscillatorVisualiser visualiser{ myLookAndFeel };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultiFaderDroneAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiFaderDroneAudioProcessorEditor)
};
