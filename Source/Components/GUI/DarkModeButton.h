/*
  ==============================================================================

    DarkModeButton.h
    Created: 7 Dec 2024 7:49:53pm
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace jr
{
    /*
    A simple wrapper class so that I can use casting to determine if a ToggleButton is the DarkModeButton
    in the look and feel class, and apply special styling
    */
    class DarkModeButton : public juce::ToggleButton
    {
    public:
        DarkModeButton() : juce::ToggleButton() {}
    };
}