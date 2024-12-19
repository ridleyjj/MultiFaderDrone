/*
  ==============================================================================

    WaveShapeIcon.h
    Created: 19 Dec 2024 7:35:02am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


namespace jr
{
    class WaveShapeIcon : public juce::Component
    {
    public:
        WaveShapeIcon(int _waveShape): waveShape(_waveShape) {}

        void paint(juce::Graphics& g) override
        {
            g.setColour(juce::Colours::black);
            g.drawRect(getLocalBounds());
        }
    private:
        int waveShape{ 0 };
    };
}