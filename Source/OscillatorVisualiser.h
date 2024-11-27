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

        /*
        Uses index to return an evenly spaced point on a zero centred circle's circumference
        */
        juce::Point<float> getCircumferencePoint(int i);

        /*
        Returns a colour based on the frequency of the oscillator at the given index in the given FaderPair
        */
        juce::Colour getColourFromOsc(FaderPair& pair, int index);

        /*
        Returns the size of dot to draw based on the current level of the given oscillator at the given index in the FaderPair.
        Size will be constrained between the max Dot size and 0
        */
        float getDotSizeFromOsc(FaderPair& pair, int index);

        /*
        Returns the point to draw the dot at for oscillator at the given index in the FaderPair.
        c is the centre point of the circle, circumferencePoint is the point on a circle's circumference (with centre 0 and radius 66)
        marking the angle at which the dot should be drawn.
        */
        juce::Point<float> getPointFromOsc(FaderPair& pair, int index, juce::Point<float>& c, juce::Point<float>& circumferencePoint);

        /*
        Draws a dot representation of the oscillator at the given index in the FaderPair, using c as the centre point for the visualiser
        and circumferencePoint as the point on a 0 centred circle that matches the angle of the desired dot.
        */
        void drawDotForOsc(juce::Graphics& g, FaderPair& pair, int index, juce::Point<float>& c, juce::Point<float>& circumferencePoint);

        void drawWobble(juce::Graphics& g, juce::Point<float>& o, float size);

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
        juce::Random random{};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorVisualiser);
    };
}