/*
  ==============================================================================

    TwoHeadedSliderAttachment.cpp
    Created: 29 Nov 2024 5:11:07pm
    Author:  ridle

  ==============================================================================
*/

#include "TwoHeadedSliderAttachment.h"

void jr::TwoHeadedSliderAttachment::sliderValueChanged(juce::Slider* s)
{
    bool isLocked = getIsLocked();
    bool isMinThumbBeingDragged = slider.getThumbBeingDragged() == 1;

    if (isLocked)
    {
        float lockedRange = currentMaxValue - currentMinValue;
        if (isMinThumbBeingDragged)
        {
            if (slider.getMinValue() + lockedRange <= slider.getMaximum())
            {
                slider.setMaxValue(slider.getMinValue() + lockedRange, juce::dontSendNotification);
            }
            else
            {
                slider.setMinValue(currentMinValue, juce::dontSendNotification);
            }
            slider.setMaxValue(slider.getMinValue() + lockedRange);
        }
        else
        {
            if (slider.getMaxValue() - lockedRange >= slider.getMinimum())
            {
                slider.setMinValue(slider.getMaxValue() - lockedRange, juce::dontSendNotification);
            }
            else
            {
                slider.setMaxValue(currentMaxValue, juce::dontSendNotification);
            }
            slider.setMinValue(slider.getMaxValue() - lockedRange);
        }
    }

    minValueParamAttachment.setValueAsPartOfGesture(slider.getMinValue());
    maxValueParamAttachment.setValueAsPartOfGesture(slider.getMaxValue());

    slider.updateText();

    currentMinValue = slider.getMinValue();
    currentMaxValue = slider.getMaxValue();
}