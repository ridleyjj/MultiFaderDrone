/*
  ==============================================================================

    NoValueColourSlider.h
    Created: 21 Dec 2024 2:25:41pm
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace jr
{
    /*
    A simple wrapper class for a juce::Slider that can be used to apply specific styling in the LookAndFeel using synamic casting.
    This class will use the same colour for the value track as the rest of the slider, so that it doesn't have a specific 'amount' shown,
    rather than just the slider position
    */
    class NoValueColourSlider : public juce::Slider {
    public:
        NoValueColourSlider() : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::NoTextBox) {}

        NoValueColourSlider(juce::Slider::SliderStyle _style, juce::Slider::TextEntryBoxPosition _textBoxPos) : juce::Slider(_style, _textBoxPos) {}
    };
}