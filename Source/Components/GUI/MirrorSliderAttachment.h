/*
  ==============================================================================

    StereoWidthAttachment.h
    Created: 29 Nov 2024 7:56:06am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../Utils/jr_utils.h"

namespace jr
{
    /*
    A custom Slider Attachment class that can be used with a two headed slider where the two heads mirror each other around a zero point.
    For example a slider that ranges from -1 to 1 where the two slider heads mirror each other with an equal absolute value but opposite sign
    */
    class MirrorSliderAttachment : public juce::Slider::Listener
    {
    public:
        MirrorSliderAttachment(juce::RangedAudioParameter& parameter, juce::Slider& _slider, juce::UndoManager* undoManager = nullptr)
            : paramAttachment(parameter, updateSlider, undoManager), slider(_slider)
        {
            slider.addListener(this);
            paramAttachment.sendInitialUpdate();
        }

        void sliderValueChanged(juce::Slider* _slider) override;

        void sliderDragStarted(juce::Slider* s) override { paramAttachment.beginGesture(); };

        void sliderDragEnded(juce::Slider* s) override { paramAttachment.endGesture(); };

    private:

        /*
        Callback that updates the Slider in the UI based on new parameter value
        keeps the two Slider heads mirroring each other based on the absolute value of the new value
        */
        std::function<void(float)> updateSlider
        {
            [&](float newValue)
            {
                newValue = abs(newValue);
                newValue = jr::Utils::constrainFloat(newValue, 0.0f, slider.getMaximum());
                slider.setMaxValue(newValue, juce::dontSendNotification);
                slider.setMinValue(newValue * -1, juce::dontSendNotification);
            }
        };

        juce::Slider& slider;
        juce::ParameterAttachment paramAttachment;
    };
}