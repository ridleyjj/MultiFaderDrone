/*
  ==============================================================================

    OscillatorVisualiser.cpp
    Created: 25 Nov 2024 3:39:49pm
    Author:  ridle

  ==============================================================================
*/

#include "OscillatorVisualiser.h"
#include "../../Utils/jr_utils.h"
#include "../../Utils/jr_juce_utils.h"

void jr::OscillatorVisualiser::resized()
{
    // update size based on bounds
    float visualiserSize = juce::jmin(getBounds().getHeight(), getBounds().getWidth());
    maxDotSize = visualiserSize * 0.08f;
    maxRadius = visualiserSize * 0.01f;
    minRadius = visualiserSize * 0.0005f;

    relativeCentre = getLocalBounds().getCentre().toFloat();
}

void jr::OscillatorVisualiser::paint(juce::Graphics& g)
{
    bool direction = true;
    int pointIndex = 0;
    for (int i{}; i < oscs.get()->size(); i++)
    {
        FaderPairs::RandomOsc& osc = oscs.get()->at(i);
        if (osc.getIsSilenced())
        {
            continue; // skip if voice isn't playing
        }
        
        auto p = getCircumferencePoint(pointIndex);

        drawDotForOsc(g, osc, direction, p);

        direction = !direction; // alternate direction
        
        if (i % 2 > 0)
        {
            pointIndex++;
        }
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

juce::Colour jr::OscillatorVisualiser::getColourFromOsc(FaderPairs::RandomOsc& osc)
{
    auto oscNormalisedFreq = (osc.getOscFrequency() - minFreq) / maxFreq;
    return lookAndFeel.getVisualiserColour(oscNormalisedFreq);
}

float jr::OscillatorVisualiser::getDotSizeFromOsc(FaderPairs::RandomOsc& osc)
{
    auto size = maxDotSize * osc.getNormalisedOscLevel();
    return jr::Utils::constrainFloat(size, 0.0f, maxDotSize);
}

juce::Point<float> jr::OscillatorVisualiser::getPointFromOsc(FaderPairs::RandomOsc& osc, bool direction, juce::Point<float>& circumferencePoint)
{
    float radius = minRadius + (abs(osc.getPan() - 0.5) * maxRadius);
    auto mod = direction ? 1.0f : -1.0f; // determines which direction point should be from centre based on index
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
        for (int i{}; i < 4; i++)
        {
            g.fillEllipse(juce::Rectangle<float>(wobbleSize, wobbleSize).withCentre(addRandomNoiseToPoint(p, centreOffset)));
        }
    }

    g.fillEllipse(juce::Rectangle<float>(size, size).withCentre(p));
}

void jr::OscillatorVisualiser::drawSpikes(juce::Graphics& g, juce::Point<float>& p, float size, FaderPairs::RandomOsc& osc)
{
    // only draw wobbles if more than 2 oscs to give effect like it is caused by interference
    if (numActivePairs > 1)
    {
        if (osc.getWaveShape() < 0.01f) { return; }

        float amount = 0.5f + 0.125f * osc.getWaveShape();
        float wobbleSize = size * amount;

        for (int i{}; i < 10; i++)
        {
            drawTriangle(g, wobbleSize, p, random.nextFloat() * 2.1f);
        }
    }
}

void jr::OscillatorVisualiser::drawTriangle(juce::Graphics& g, float r, juce::Point<float>& c, float offset)
{
    auto thirdTwoPi = juce::MathConstants<float>::twoPi / 3.0f;
    offset = fmod(offset, thirdTwoPi);

    auto point1 = jr::JuceUtils::getPointOnCircle(r, offset + 0.0f, c);
    auto point2 = jr::JuceUtils::getPointOnCircle(r, offset + thirdTwoPi, c);
    auto point3 = jr::JuceUtils::getPointOnCircle(r, offset + thirdTwoPi * 2.0f, c);
    juce::Path triangle{};
    triangle.addTriangle(point1, point2, point3);
    g.fillPath(triangle);
}

void jr::OscillatorVisualiser::drawDotForOsc(juce::Graphics& g, FaderPairs::RandomOsc& osc, bool direction, juce::Point<float>& circumferencePoint)
{
    g.setColour(getColourFromOsc(osc));
    drawWobble(g, getPointFromOsc(osc, direction, circumferencePoint), getDotSizeFromOsc(osc));
    drawSpikes(g, getPointFromOsc(osc, direction, circumferencePoint), getDotSizeFromOsc(osc), osc);
}