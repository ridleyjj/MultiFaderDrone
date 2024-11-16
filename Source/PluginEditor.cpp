/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StyleSheet.h"

//==============================================================================
MuiltFaderDroneAudioProcessorEditor::MuiltFaderDroneAudioProcessorEditor (MuiltFaderDroneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&myLookAndFeel);
    myLookAndFeel.setColour(juce::Label::textColourId, juce::Colours::black);
    myLookAndFeel.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    myLookAndFeel.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black.withAlpha(0.0f));
    myLookAndFeel.setDefaultSansSerifTypeface(StyleSheet::boldFont);

    // sliders

    initSimpleSlider(&voicesSlider, &voicesLabel, "Num Voices", 2, 30, 2);
    initSimpleSlider(&gainSlider, &gainLabel, "Master Gain", 0.0, 1.0, 0.01);
    initSimpleSlider(&lfoRateSlider, &lfoRateLabel, "Rate", 0.0, 1.0, 0.01);
    initSimpleSlider(&stereoSlider, &stereoLabel, "Width", -1.0, 1.0, 0.01);
    stereoSlider.setMinAndMaxValues(0.0, 0.0, juce::dontSendNotification);
    initSimpleSlider(&freqRangeSlider, &freqRangeLabel, "Range", 80.0f, 2000.0f, 5.0f);
    freqRangeSlider.setMinAndMaxValues(120.0f, 1200.0f, juce::dontSendNotification);
    freqRangeSlider.textFromValueFunction = [=](double value)
        {
            return juce::String(freqRangeSlider.getMinValue()) + "Hz - " + juce::String(freqRangeSlider.getMaxValue()) + "Hz";
        };
    freqRangeSlider.updateText();

    

    gainSlider.setValue(1.0, juce::dontSendNotification);
    gainSlider.setName("gain");

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 450);
}

void MuiltFaderDroneAudioProcessorEditor::initSimpleSlider(juce::Slider* slider, juce::Label* label, const juce::String& name, double minVal, double maxVal, double step) {
    slider->setTextBoxIsEditable(false);
    slider->setRange(minVal, maxVal, step);
    slider->addListener(this);
    slider->sendLookAndFeelChange();
    addAndMakeVisible(slider);

    label->setText(name, juce::NotificationType::dontSendNotification);
    label->setJustificationType(juce::Justification::centredTop);
    label->attachToComponent(slider, false);
}

MuiltFaderDroneAudioProcessorEditor::~MuiltFaderDroneAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void MuiltFaderDroneAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Colour beige{ juce::Colour(240, 227, 214) };

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(beige);
}

void MuiltFaderDroneAudioProcessorEditor::resized()
{
    voicesSlider.setBoundsRelative(0.35f, 0.5f, 0.3f, 0.3f);

    gainSlider.setBoundsRelative(0.35f, 0.1f, 0.3f, 0.3f);

    freqRangeSlider.setBoundsRelative(0.1f, 0.1f, 0.2f, 0.7f);

    lfoRateSlider.setBoundsRelative(0.75f, 0.1f, 0.1f, 0.7f);

    stereoSlider.setBoundsRelative(0.1f, 0.9f, 0.8f, 0.1f);
}

void MuiltFaderDroneAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &voicesSlider) {
        DBG(voicesSlider.getValue());
        audioProcessor.setOscCount(voicesSlider.getValue());
    }
    else if (slider == &lfoRateSlider) {
        audioProcessor.setLfoRate(lfoRateSlider.getValue());
    }
    else if (slider == &freqRangeSlider)
    {
        freqRangeSlider.updateText();
        audioProcessor.setOscFreqRange(freqRangeSlider.getMinValue(), freqRangeSlider.getMaxValue());
    }
    else if (slider == &gainSlider)
    {
        audioProcessor.setGain(gainSlider.getValue());
    }
    else if (slider == &stereoSlider)
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
}