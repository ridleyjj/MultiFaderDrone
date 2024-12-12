/*
  ==============================================================================

    TwoHeadedSliderAttachment.h
    Created: 29 Nov 2024 5:11:07pm
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace jr
{
    /*
    A class that allows for a two-headed JUCE slider to be attached to two distinct Audio Parameters in the processor.
    Includes functionality for 'locking' the two heads so that they move together and remain at a constant range
    */
    class TwoHeadedSliderAttachment : public juce::Slider::Listener
    {
    public:

        /*
        Note - getIsLocked is a callback that returns whether or not the slider is currently 'locked' in that the two heads are moving together, by default always returns false
        */
        TwoHeadedSliderAttachment(juce::RangedAudioParameter& minParameter, juce::RangedAudioParameter& maxParameter, juce::Slider& _slider,
            std::function<bool(void)> _getIsLocked = []() { return false; }, juce::UndoManager* undoManager = nullptr)
            : slider(_slider), minValueParamAttachment(minParameter, updateSliderMinValue, undoManager), maxValueParamAttachment(maxParameter, updateSliderMaxValue, undoManager),
            getIsLocked(_getIsLocked)
        {
            currentMinValue = minParameter.getNormalisableRange().convertFrom0to1(minParameter.getValue());
            currentMaxValue = maxParameter.getNormalisableRange().convertFrom0to1(maxParameter.getValue());

            slider.addListener(this);

            /* important to sendInitialUpdate for maxValue first, as they both start initially at 0 and therefore if we try to set the min value to anything
            above 0 before setting the max value, the slider will limit the min value to 0 as the min head cannot surpass the max head */
            maxValueParamAttachment.sendInitialUpdate();
            minValueParamAttachment.sendInitialUpdate();

            slider.updateText();
        }

        /*
        Update from GUI to parameter state - needs to take minValue and maxValue of slider and set to params.
        If getIsLocked returns true, 
        */
        void sliderValueChanged(juce::Slider* _slider) override;

        /*
        Begins gesture for relevant paramAttachment, or both if slider is locked
        */
        void sliderDragStarted(juce::Slider* s) override
        {
            minValueParamAttachment.beginGesture();
            maxValueParamAttachment.beginGesture();
        }

        void sliderDragEnded(juce::Slider* s) override
        {
            minValueParamAttachment.endGesture();
            maxValueParamAttachment.endGesture();
        }

    private:
        juce::Slider& slider;

        /*
        Updates slider from AudioParameter change
        */
        std::function<void(float)> updateSliderMinValue { [&](float newValue)
            { 
                slider.setMinValue(newValue, juce::dontSendNotification);
                currentMinValue = slider.getMinValue();
            }
        };

        /*
        Updates slider from AudioParameter change
        */
        std::function<void(float)> updateSliderMaxValue{ [&](float newValue)
            {
                slider.setMaxValue(newValue, juce::dontSendNotification);
                currentMaxValue = slider.getMaxValue();
            }
        };

        juce::ParameterAttachment minValueParamAttachment;
        juce::ParameterAttachment maxValueParamAttachment;
        std::function<bool(void)> getIsLocked;
        float currentMinValue;
        float currentMaxValue;
    };
}