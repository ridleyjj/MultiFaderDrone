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
    initSimpleSlider(&lfoRateSlider, &lfoRateLabel, "Rate", 0.0, 1.0, 0.01);
    initSimpleSlider(&freqSpreadSlider, &freqSpreadLabel, "Spread", -1.0f, 1.0f, 0.01f);
    initSimpleSlider(&freqRangeSlider, &freqRangeLabel, "Range", 80.0f, 2000.0f, 5.0f);
    freqRangeSlider.setMinAndMaxValues(120.0f, 1200.0f, juce::dontSendNotification);
    freqRangeSlider.textFromValueFunction = [&](double value)
        {
            return juce::String(freqRangeSlider.getMinValue()) + "Hz - " + juce::String(freqRangeSlider.getMaxValue()) + "Hz";
        };
    freqRangeSlider.updateText();

    freqSpreadSlider.setValue(0.0f);

    // buttons

    resetButton.addListener(this);
    addAndMakeVisible(resetButton);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);
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
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    voicesSlider.setBoundsRelative(0.35f, 0.5f, 0.3f, 0.3f);

    resetButton.setBoundsRelative(0.4f, 0.1f, 0.2f, 0.1f);

    freqRangeSlider.setBoundsRelative(0.2f, 0.1f, 0.2f, 0.8f);

    freqSpreadSlider.setBoundsRelative(0.0f, 0.1f, 0.2f, 0.8f);

    lfoRateSlider.setBoundsRelative(0.75f, 0.1f, 0.1f, 0.8f);
}

void MuiltFaderDroneAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &voicesSlider) {
        DBG(voicesSlider.getValue());
        audioProcessor.setOscCount(voicesSlider.getValue());
    }
    else if (slider == &freqSpreadSlider) {
        audioProcessor.setFrequencySpread(freqSpreadSlider.getValue() * 0.1f);
    }
    else if (slider == &lfoRateSlider) {
        audioProcessor.setLfoRate(lfoRateSlider.getValue());
    }
    else if (slider == &freqRangeSlider)
    {
        freqRangeSlider.updateText();
        audioProcessor.setOscFreqRange(freqRangeSlider.getMinValue(), freqRangeSlider.getMaxValue());
    }
}

void MuiltFaderDroneAudioProcessorEditor::buttonClicked(juce::Button* button) {
    if (button == &resetButton) {
        audioProcessor.resetFreqs();
        freqSpreadSlider.setValue(0.0f, juce::dontSendNotification);
        lfoRateSlider.setValue(0.0f, juce::dontSendNotification);
    }
}