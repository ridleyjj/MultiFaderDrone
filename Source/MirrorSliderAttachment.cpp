/*
  ==============================================================================

    StereoWidthAttachment.cpp
    Created: 29 Nov 2024 7:56:07am
    Author:  ridle

  ==============================================================================
*/

#include "MirrorSliderAttachment.h"

void jr::MirrorSliderAttachment::sliderValueChanged(juce::Slider* _slider)
{
    if (_slider != &slider)
        return;

    float newValue{};

    int thumbIndex = slider.getThumbBeingDragged();
    if (1 == thumbIndex)
    {
        // constraints stop either slider head from going passed zero into other heads region
        newValue = abs(jr::Utils::constrainFloat(slider.getMinValue(), slider.getMinimum(), 0.0f));
    } else
    {
        newValue = jr::Utils::constrainFloat(slider.getMaxValue(), 0.0f, slider.getMaximum());
    }

    slider.setMinAndMaxValues(newValue * -1.0, newValue, juce::dontSendNotification); // ensures GUI blocks sliders going past centre point

    paramAttachment.setValueAsPartOfGesture(newValue);
}