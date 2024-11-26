/*
  ==============================================================================

    OscillatorVisualiser.h
    Created: 25 Nov 2024 3:39:49pm
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>
#include "jr_FaderPairs.h"

namespace jr
{
    class OscillatorVisualiser : public juce::Component
    {
    public:
        OscillatorVisualiser() {}
        
        void setPairs(std::shared_ptr<std::vector<FaderPair>> _pairs) { pairs = _pairs; }

        void paint(juce::Graphics&) override;

    private:
        int maxNumDots{ 16 };
        int halfMaxDots{ maxNumDots / 2 };
        std::vector<juce::Point<float>> circlePoints
        {
            juce::Point<float>(66.0f, 0.0f),
            juce::Point<float>(0.0f, 66.0f),
            juce::Point<float>(46.5f, -46.5f),
            juce::Point<float>(46.5f, 46.5f),
            juce::Point<float>(60.2f, -27.0f),
            juce::Point<float>(27.0f, 60.2f),
            juce::Point<float>(27.0f, -60.2f),
            juce::Point<float>(60.2f, 27.0f),
        };
        std::shared_ptr<std::vector<FaderPair>> pairs{ nullptr };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorVisualiser);
    };
}