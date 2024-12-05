/*
  ==============================================================================

    OscillatorVisualiser.cpp
    Created: 25 Nov 2024 3:39:49pm
    Author:  ridle

  ==============================================================================
*/

#include "OscillatorVisualiser.h"
#include "../../LookAndFeel/StyleSheet.h"

void jr::OscillatorVisualiser::resized()
{
    // update size based on bounds
    float visualiserSize = juce::jmin(getBounds().getHeight(), getBounds().getWidth());
    maxDotSize = visualiserSize * 0.08f;
    maxRadius = visualiserSize * 0.01f;
    minRadius = visualiserSize * 0.0005f;

    relativeCentre = getLocalBounds().getCentre().toFloat();;
}

void jr::OscillatorVisualiser::paint(juce::Graphics& g)
{
    for (int i{}; i < pairs.get()->size(); i++)
    {
        FaderPair& pair = pairs.get()->at(i);
        if (pair.getIsSilenced())
        {
            continue; // skip if voice isn't playing
        }
        
        auto p = getCircumferencePoint(i);

        drawDotForOsc(g, pair, 0, p);
        drawDotForOsc(g, pair, 1, p);
    }
}

juce::Point<float> jr::OscillatorVisualiser::getCircumferencePoint(int i)
{
    // circle array is only half points on a circle to save space, so each time the index exceeds a multiple of the array size,
    // flip between multiplying by 1 and -1 in order to cover a whole circle
    auto rem = i % circlePoints.size();
    auto mod = -1.0f;
    if (rem < 1.0f || (rem >= 2.0f && rem < 3.0f))
    {
        mod = 1.0f;
    }

    int index = i;
    // wrap index into circlePoints array
    while (index >= circlePoints.size())
    {
        index -= circlePoints.size();
    }
    return circlePoints.at(index);
}

juce::Colour jr::OscillatorVisualiser::getColourFromOsc(FaderPair& pair, int index)
{
    auto oscNormalisedFreq = (pair.getOscFrequency(index) - minFreq) / maxFreq;
    return CustomLookAndFeel::getVisualiserColour(oscNormalisedFreq);
}

float jr::OscillatorVisualiser::getDotSizeFromOsc(FaderPair& pair, int index)
{
    auto size = maxDotSize * pair.getNormalisedOscLevel(index);
    return jr::Utils::constrainFloat(size, 0.0f, maxDotSize);
}

juce::Point<float> jr::OscillatorVisualiser::getPointFromOsc(FaderPair& pair, int index, juce::Point<float>& circumferencePoint)
{
    float radius = minRadius + (abs(pair.getPan(index) - 0.5) * maxRadius);
    auto mod = index == 0 ? 1.0f : -1.0f; // determines which direction point should be from centre based on index
    return relativeCentre + (circumferencePoint * radius) * mod;
}

juce::Point<float> jr::OscillatorVisualiser::addRandomNoiseToPoint(juce::Point<float> p, float scale)
{
    return juce::Point<float>(p.getX() + ((random.nextFloat() - 0.5f) * scale), p.getY() + ((random.nextFloat() - 0.5f) * scale));
}

void jr::OscillatorVisualiser::drawWobble(juce::Graphics& g, juce::Point<float>& p, float size)
{
    // only draw wobbles if more than 2 oscs to give effect like it is caused by interference
    if (numActivePairs > 1)
    {
        float wobbleSize = size * 0.8f;
        float centreOffset = size * 0.25f;

        // draw 3 extra random circles with noise from centre to create effect like the dot is shaking
        for (int i{}; i < 3; i++)
        {
            g.fillEllipse(juce::Rectangle<float>(wobbleSize, wobbleSize).withCentre(addRandomNoiseToPoint(p, centreOffset)));
        }
    }

    g.fillEllipse(juce::Rectangle<float>(size, size).withCentre(p));
}

void jr::OscillatorVisualiser::drawDotForOsc(juce::Graphics& g, FaderPair& pair, int index, juce::Point<float>& circumferencePoint)
{
    g.setColour(getColourFromOsc(pair, index));
    drawWobble(g, getPointFromOsc(pair, index, circumferencePoint), getDotSizeFromOsc(pair, index));
}