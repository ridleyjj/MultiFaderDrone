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
    audioProcessor.setPrevRangeMin(freqRangeSlider.getMinValue());
    audioProcessor.setPrevRangeMax(freqRangeSlider.getMaxValue());

    freqRangeSlider.setColour(juce::Slider::trackColourId, myLookAndFeel.getValueTrackColour(audioProcessor.getRangeFrozen()));

    // APVTS Attachments

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::GAIN.toString(), gainSlider);
    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::RATE.toString(), lfoRateSlider);
    voicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), ID::NUM_VOICES.toString(), voicesSlider);

    // buttons

    freezeRangeButton.sendLookAndFeelChange(); // needed to receive latest look and feel font
    freezeRangeButton.onClick = [this] { toggleRangeFrozen(); };
    //rangeFrozen = audioProcessor.getRangeFrozen();
    if (audioProcessor.getRangeFrozen())
    {
        freqRangeSlider.setColour(juce::Slider::trackColourId, myLookAndFeel.getValueTrackColour(true));
    }
    addAndMakeVisible(freezeRangeButton);

    freezeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getAPVTS(), ID::FREEZE_RANGE.toString(), freezeRangeButton);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 450);

    visualiser.setPairs(audioProcessor.getPairs());
    addAndMakeVisible(visualiser);

    startTimerHz(24); // FPS
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
    voicesSlider.setBoundsRelative(0.175f, 0.5f, 0.15f, 0.3f);

    gainSlider.setBoundsRelative(0.175f, 0.1f, 0.15f, 0.3f);

    freqRangeSlider.setBoundsRelative(0.05f, 0.1f, 0.1f, 0.7f);

    freezeRangeButton.setBoundsRelative(0.05f, 0.8f, 0.1f, 0.1f);

    lfoRateSlider.setBoundsRelative(0.375f, 0.1f, 0.05f, 0.7f);

    stereoSlider.setBoundsRelative(0.05f, 0.9f, 0.4f, 0.1f);

    visualiser.setBounds(400, 0, 400, 450);
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
    if (audioProcessor.getRangeFrozen())
    {
        if (freqRangeSlider.getMinValue() != audioProcessor.getPrevMinRange())
        {
            if (freqRangeSlider.getMinValue() + audioProcessor.getFrozenRangeAmount() <= freqRangeSlider.getMaximum())
            {
                freqRangeSlider.setMaxValue(freqRangeSlider.getMinValue() + audioProcessor.getFrozenRangeAmount(), juce::dontSendNotification);
            }
            else
            {
                freqRangeSlider.setMinValue(audioProcessor.getPrevMinRange(), juce::dontSendNotification);
            }
        }
        else if (freqRangeSlider.getMaxValue() != audioProcessor.getPrevMaxRange())
        {
            if (freqRangeSlider.getMaxValue() - audioProcessor.getFrozenRangeAmount() >= freqRangeSlider.getMinimum())
            {
                freqRangeSlider.setMinValue(freqRangeSlider.getMaxValue() - audioProcessor.getFrozenRangeAmount(), juce::dontSendNotification);
            }
            else
            {
                freqRangeSlider.setMaxValue(audioProcessor.getPrevMaxRange(), juce::dontSendNotification);
            }
        }
    }

    audioProcessor.setPrevRangeMin(freqRangeSlider.getMinValue());
    audioProcessor.setPrevRangeMax(freqRangeSlider.getMaxValue());

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
    freqRangeSlider.setColour(juce::Slider::trackColourId, CustomLookAndFeel::getValueTrackColour(audioProcessor.getRangeFrozen()));
}