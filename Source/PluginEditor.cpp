/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LookAndFeel/StyleSheet.h"

//==============================================================================
MultiFaderDroneAudioProcessorEditor::MultiFaderDroneAudioProcessorEditor (MultiFaderDroneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&myLookAndFeel);
    setLookAndFeel(&myLookAndFeel);

    // sliders

    initSimpleSlider(&voicesSlider, &voicesLabel, "Num Voices");
    initSimpleSlider(&gainSlider, &gainLabel, "Master Gain");
    initSimpleSlider(&lfoRateSlider, &lfoRateLabel, "Rate");
    initSimpleSliderWithRange(&stereoSlider, &stereoLabel, "Stereo Width", -1.0, 1.0, 0.01);
    initSimpleSliderWithRange(&freqRangeSlider, &freqRangeLabel, "Range", 80.0f, 2000.0f, 5.0f);
    
    // Two-Headed Slider specifics

    freqRangeSlider.textFromValueFunction = [=](double value)
        {
            return juce::String(freqRangeSlider.getMinValue()) + "Hz - " + juce::String(freqRangeSlider.getMaxValue()) + "Hz";
        };

    freqRangeSlider.setGetIsLockedCallback([&]() { return audioProcessor.getRangeLocked();  });

    // APVTS Attachments

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::GAIN.toString(), gainSlider);
    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::RATE.toString(), lfoRateSlider);
    voicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::NUM_VOICES.toString(), voicesSlider);
    stereoWidthAttachment = std::make_unique<jr::MirrorSliderAttachment>(*(audioProcessor.getAPVTS().getParameter(ID::STEREO_WIDTH)), stereoSlider);
    freqRangeAttachment = std::make_unique<jr::TwoHeadedSliderAttachment>(*(audioProcessor.getAPVTS().getParameter(ID::FREQ_RANGE_MIN)),
        *(audioProcessor.getAPVTS().getParameter(ID::FREQ_RANGE_MAX)), freqRangeSlider, [&]() { return audioProcessor.getRangeLocked(); });

    // buttons

    lockRangeButton.sendLookAndFeelChange(); // needed to receive latest look and feel font
    lockRangeButton.onClick = [&]() { freqRangeSlider.repaint(); }; // ensures that slider is refreshed whenever lock button is toggled
    addAndMakeVisible(lockRangeButton);

    darkModeButton.addListener(this);
    addAndMakeVisible(darkModeButton);

    lockRangeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getAPVTS(), ID::LOCK_RANGE.toString(), lockRangeButton);
    darkModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getAPVTS(), ID::DARK_MODE.toString(), darkModeButton);

    visualiser.setPairs(audioProcessor.getOscs());
    addAndMakeVisible(visualiser);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 500);

    startTimerHz(24); // FPS
}

void MultiFaderDroneAudioProcessorEditor::initSimpleSlider(juce::Slider* slider, juce::Label* label, const juce::String& name) {
    // slider init
    slider->setColour(juce::Slider::trackColourId, myLookAndFeel.getValueTrackColour(false));
    slider->setTextBoxIsEditable(false);
    slider->sendLookAndFeelChange(); // needed to receive latest look and feel font
    addAndMakeVisible(slider);

    // text label init
    label->setText(name, juce::NotificationType::dontSendNotification);
    label->setJustificationType(juce::Justification::centredTop);
    label->attachToComponent(slider, false);
}

void MultiFaderDroneAudioProcessorEditor::initSimpleSliderWithRange(juce::Slider* slider, juce::Label* label, const juce::String& name, double minVal, double maxVal, double step) {
    initSimpleSlider(slider, label, name);

    slider->setRange(minVal, maxVal, step);
}

MultiFaderDroneAudioProcessorEditor::~MultiFaderDroneAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    stopTimer();
}

//==============================================================================
void MultiFaderDroneAudioProcessorEditor::timerCallback()
{
    visualiser.setPairs(audioProcessor.getPairs());
    visualiser.setNumActivePairs(voicesSlider.getValue());
    visualiser.repaint();
}

void MultiFaderDroneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(myLookAndFeel.getBackgroundColour());
}

void MultiFaderDroneAudioProcessorEditor::resized()
{
    voicesSlider.setBoundsRelative(0.55f, 0.1f, 0.25f, 0.25f);

    gainSlider.setBoundsRelative(0.2f, 0.1f, 0.25f, 0.25f);

    freqRangeSlider.setBoundsRelative(0.02f, 0.1f, 0.2f, 0.65f);

    lfoRateSlider.setBoundsRelative(0.78f, 0.1f, 0.2f, 0.65f);

    lockRangeButton.setBoundsRelative(0.03f, 0.75f, 0.4f, 0.1f);

    stereoSlider.setBoundsRelative(0.02f, 0.9f, 0.96f, 0.1f);

    visualiser.setBoundsRelative(0.25f, 0.33f, 0.5f, 0.5f);

    darkModeButton.setBoundsRelative(0.92f, 0.0f, 0.08f, 0.08f);
}

void MultiFaderDroneAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &darkModeButton)
    {
        myLookAndFeel.setIsDarkMode(darkModeButton.getToggleStateValue().getValue());
        refreshStyles();
    }
}

void MultiFaderDroneAudioProcessorEditor::refreshStyles()
{
    sendNewLookAndFeel();
    repaint();
}

void MultiFaderDroneAudioProcessorEditor::sendNewLookAndFeel()
{
    lockRangeButton.sendLookAndFeelChange();
    freqRangeSlider.sendLookAndFeelChange();
    lfoRateSlider.sendLookAndFeelChange();
    voicesSlider.sendLookAndFeelChange();
    gainSlider.sendLookAndFeelChange();
    stereoSlider.sendLookAndFeelChange();
    voicesLabel.sendLookAndFeelChange();
    lfoRateLabel.sendLookAndFeelChange();
    freqRangeLabel.sendLookAndFeelChange();
    gainLabel.sendLookAndFeelChange();
    stereoLabel.sendLookAndFeelChange();
}