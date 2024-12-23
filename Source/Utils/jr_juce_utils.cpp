/*
  ==============================================================================

    jr_juce_utils.cpp
    Created: 23 Dec 2024 11:55:29am
    Author:  ridle

  ==============================================================================
*/

#include "jr_juce_utils.h"
#include "JuceHeader.h"

juce::Point<float> jr::JuceUtils::getPointOnCircle(float r, float theta, juce::Point<float> c)
{
    return juce::Point<float>(
        c.getX() + r * juce::dsp::FastMathApproximations::cos(theta),
        c.getY() + r * juce::dsp::FastMathApproximations::sin(theta)
    );
}