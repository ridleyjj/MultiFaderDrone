/*
  ==============================================================================

    OscillatorVisualiser.cpp
    Created: 25 Nov 2024 3:39:49pm
    Author:  ridle

  ==============================================================================
*/

#include "OscillatorVisualiser.h"
#include "StyleSheet.h"

void jr::OscillatorVisualiser::paint(juce::Graphics& g)
{
    // using getBounds().getCentre() returns the centre relative to the whole plugin (600, 225) whereas below returns centre relative to
    // visualiser start point (400, 225) which results in desired centre when drawing
    juce::Point center = juce::Point(getBounds().getWidth() / 2.0f, getBounds().getHeight() / 2.0f);

    for (int i{}; i < pairs.get()->size(); i++)
    {
        FaderPair& pair = pairs.get()->at(i);
        if (pair.getIsSilenced())
        {
            continue; // skip if voice isn't playing
        }
        
        auto p = getCircumferencePoint(i);

        drawDotForOsc(g, pair, 0, center, p);
        drawDotForOsc(g, pair, 1, center, p);
    }
}

juce::Point<float> jr::OscillatorVisualiser::getCircumferencePoint(int i)
{
    auto rem = i % circlePoints.size();
    auto mod = -1.0f;
    if (rem < 1.0f || (rem >= 2.0f && rem < 3.0f))
    {
        mod = 1.0f;
    }
    int index = i;
    while (index >= circlePoints.size())
    {
        index -= circlePoints.size();
    }
    return circlePoints.at(index);
}

juce::Colour jr::OscillatorVisualiser::getColourFromOsc(FaderPair& pair, int index)
{
    auto minFreq = 80.0f;
    auto maxFreq = 1600.0f;
    auto oscNormalisedFreq = (pair.getOscFrequency(index) - minFreq) / maxFreq;
    return CustomLookAndFeel::getVisualiserColour(oscNormalisedFreq);
}

float jr::OscillatorVisualiser::getDotSizeFromOsc(FaderPair& pair, int index)
{
    float maxDotSize = 28.0f;
    auto size = maxDotSize * pair.getNormalisedOscLevel(index);
    return jr::Utils::constrainFloat(size, 0.0f, maxDotSize);
}

juce::Point<float> jr::OscillatorVisualiser::getPointFromOsc(FaderPair& pair, int index, juce::Point<float>& c, juce::Point<float>& circumferencePoint)
{
    float radius = 0.2f + (abs(pair.getPan(index) - 0.5) * 4.0f);
    auto mod = index == 0 ? 1.0f : -1.0f; // determines which direction point should be from centre based on index
    return c + (circumferencePoint * radius) * mod;
}

void jr::OscillatorVisualiser::drawWobble(juce::Graphics& g, juce::Point<float>& o, float size)
{
    float wobbleSize = size * 0.8f;
    float centreOffset = size * 0.25f;

    auto getRandomCenter = [&]() { return juce::Point<float>(o.getX() + ((random.nextFloat() - 0.5f) * centreOffset), o.getY() + ((random.nextFloat() - 0.5f) * centreOffset)); };

    for (int i{}; i < 3; i++)
    {
        auto extra1 = juce::Rectangle<float>(wobbleSize, wobbleSize).withCentre(getRandomCenter());
        g.fillEllipse(extra1);
    }
    auto mainDot = juce::Rectangle<float>(size, size).withCentre(o);
    g.fillEllipse(mainDot);
}

void jr::OscillatorVisualiser::drawDotForOsc(juce::Graphics& g, FaderPair& pair, int index, juce::Point<float>& c, juce::Point<float>& circumferencePoint)
{
    g.setColour(getColourFromOsc(pair, index));
    auto size = getDotSizeFromOsc(pair, index);
    auto pos = getPointFromOsc(pair, index, c, circumferencePoint);

    drawWobble(g, pos, size);
}