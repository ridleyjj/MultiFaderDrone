/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StyleSheet.h"

//==============================================================================
MultiFaderDroneAudioProcessorEditor::MultiFaderDroneAudioProcessorEditor (MultiFaderDroneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&myLookAndFeel);

    // sliders

    initSimpleSlider(&voicesSlider, &voicesLabel, "Num Voices");
    initSimpleSlider(&gainSlider, &gainLabel, "Master Gain");
    initSimpleSlider(&lfoRateSlider, &lfoRateLabel, "Rate");
    initSimpleSliderWithRange(&stereoSlider, &stereoLabel, "Width", -1.0, 1.0, 0.01);
    initSimpleSliderWithRange(&freqRangeSlider, &freqRangeLabel, "Range", 80.0f, 2000.0f, 5.0f);
    
    // Two-Headed Slider specifics

    float currentStereoWidth = audioProcessor.getStereoWidth();
    stereoSlider.setMinAndMaxValues(currentStereoWidth * -1, currentStereoWidth, juce::dontSendNotification);

    freqRangeSlider.setMinAndMaxValues(audioProcessor.getCurrentFreqRangeMin(), audioProcessor.getCurrentFreqRangeMax(), juce::dontSendNotification);
    freqRangeSlider.textFromValueFunction = [=](double value)
        {
            return juce::String(freqRangeSlider.getMinValue()) + "Hz - " + juce::String(freqRangeSlider.getMaxValue()) + "Hz";
        };
    freqRangeSlider.updateText();
    prevRangeMin = freqRangeSlider.getMinValue();
    prevRangeMax = freqRangeSlider.getMaxValue();

    freqRangeSlider.setColour(juce::Slider::trackColourId, myLookAndFeel.getValueTrackColour(rangeFrozen));

    // APVTS Attachments

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::GAIN.toString(), gainSlider);
    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::RATE.toString(), lfoRateSlider);
    voicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::NUM_VOICES.toString(), voicesSlider);

    // buttons

    freezeRangeButton.sendLookAndFeelChange(); // needed to receive latest look and feel font
    freezeRangeButton.onClick = [this] { toggleRangeFrozen(); };
    rangeFrozen = audioProcessor.getRangeFrozen();
    if (rangeFrozen)
    {
        frozenRangeAmount = freqRangeSlider.getMaxValue() - freqRangeSlider.getMinValue();
        freqRangeSlider.setColour(juce::Slider::trackColourId, myLookAndFeel.getValueTrackColour(rangeFrozen));
    }
    freezeRangeButton.setToggleState(rangeFrozen, juce::dontSendNotification);
    addAndMakeVisible(freezeRangeButton);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 450);
}

void MultiFaderDroneAudioProcessorEditor::initSimpleSlider(juce::Slider* slider, juce::Label* label, const juce::String& name) {
    // slider init
    slider->setColour(juce::Slider::trackColourId, CustomLookAndFeel::getValueTrackColour(false));
    slider->setTextBoxIsEditable(false);
    slider->addListener(this);
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
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void MultiFaderDroneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(myLookAndFeel.getBackgroundColour());
}

void MultiFaderDroneAudioProcessorEditor::resized()
{
    voicesSlider.setBoundsRelative(0.35f, 0.5f, 0.3f, 0.3f);

    gainSlider.setBoundsRelative(0.35f, 0.1f, 0.3f, 0.3f);

    freqRangeSlider.setBoundsRelative(0.1f, 0.1f, 0.2f, 0.7f);

    freezeRangeButton.setBoundsRelative(0.1f, 0.8f, 0.2f, 0.1f);

    lfoRateSlider.setBoundsRelative(0.75f, 0.1f, 0.1f, 0.7f);

    stereoSlider.setBoundsRelative(0.1f, 0.9f, 0.8f, 0.1f);
}

void MultiFaderDroneAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &freqRangeSlider)
    {
        freqRangeSliderUpdate();
    }
    else if (slider == &stereoSlider)
    {
        stereoSliderUpdate();
    }
}

void MultiFaderDroneAudioProcessorEditor::freqRangeSliderUpdate()
{
    // if range is frozen, move values together
    if (rangeFrozen)
    {
        if (freqRangeSlider.getMinValue() != prevRangeMin)
        {
            if (freqRangeSlider.getMinValue() + frozenRangeAmount <= freqRangeSlider.getMaximum())
            {
                freqRangeSlider.setMaxValue(freqRangeSlider.getMinValue() + frozenRangeAmount, juce::dontSendNotification);
            }
            else
            {
                freqRangeSlider.setMinValue(prevRangeMin, juce::dontSendNotification);
            }
        }
        else if (freqRangeSlider.getMaxValue() != prevRangeMax)
        {
            if (freqRangeSlider.getMaxValue() - frozenRangeAmount >= freqRangeSlider.getMinimum())
            {
                freqRangeSlider.setMinValue(freqRangeSlider.getMaxValue() - frozenRangeAmount, juce::dontSendNotification);
            }
            else
            {
                freqRangeSlider.setMaxValue(prevRangeMax, juce::dontSendNotification);
            }
        }
    }

    prevRangeMin = freqRangeSlider.getMinValue();
    prevRangeMax = freqRangeSlider.getMaxValue();

    freqRangeSlider.updateText();
    audioProcessor.setOscFreqRange(freqRangeSlider.getMinValue(), freqRangeSlider.getMaxValue());
}

void MultiFaderDroneAudioProcessorEditor::stereoSliderUpdate()
{
    float newVal{};
    if (stereoSlider.getMinValue() != prevStereoMin)
    {
        newVal = std::abs(stereoSlider.getMinValue());
    }
    else
    {
        newVal = std::abs(stereoSlider.getMaxValue());
    }
    stereoSlider.setMinAndMaxValues(newVal * -1.0, newVal, juce::dontSendNotification);

    audioProcessor.setStereoWidth(stereoSlider.getMaxValue());
    prevStereoMin = stereoSlider.getMinValue();
    prevStereoMax = stereoSlider.getMaxValue();
}

void MultiFaderDroneAudioProcessorEditor::toggleRangeFrozen()
{
    rangeFrozen = !rangeFrozen;
    audioProcessor.setRangeFrozen(rangeFrozen);
    frozenRangeAmount = rangeFrozen ? freqRangeSlider.getMaxValue() - freqRangeSlider.getMinValue() : 0.0;
    freqRangeSlider.setColour(juce::Slider::trackColourId, CustomLookAndFeel::getValueTrackColour(rangeFrozen));
}