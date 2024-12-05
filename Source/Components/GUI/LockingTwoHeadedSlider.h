/*
  ==============================================================================

    LockingTwoHeadedSlider.h
    Created: 2 Dec 2024 8:00:18am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace jr
{
    /*
    A wrapper class for a juce::Slider that is a Two Headed Slider that can be set to lock, where the range between the min
    and max heads will be kept constant
    */
    class LockingTwoHeadedSlider : public juce::Slider
    {
    public:
        LockingTwoHeadedSlider(bool isVertical, juce::Slider::TextEntryBoxPosition textBoxPos, std::function<bool(void)> _getIsLocked = []() { return false; })
            : juce::Slider(isVertical ? juce::Slider::SliderStyle::TwoValueVertical : juce::Slider::SliderStyle::TwoValueHorizontal, textBoxPos), getIsLockedCallback(_getIsLocked) {}

        /*
        Sets the callback function that will be used to determine whether the slider is locked or not at any moment
        */
        void setGetIsLockedCallback(std::function<bool(void)> _getIsLocked) { getIsLockedCallback = _getIsLocked; }
        
        bool isLocked() { return getIsLockedCallback(); }
    private:
        std::function<bool(void)> getIsLockedCallback;
    };
}