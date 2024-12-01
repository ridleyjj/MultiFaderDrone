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
        std::function<void(float)> updateSliderMinValue { [&](float newValue)
            { 
                slider.setMinValue(newValue, juce::dontSendNotification);
                currentMinValue = slider.getMinValue();
            }
        };
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