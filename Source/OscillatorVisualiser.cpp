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
    g.setColour(CustomLookAndFeel::getBackgroundColour());
    g.drawRect(juce::Rectangle<float>(0.0f, 0.0f, getBounds().getWidth(), getBounds().getHeight()));
    juce::Point center = juce::Point(getBounds().getWidth() / 2.0f, getBounds().getHeight() / 2.0f);

    g.setColour(CustomLookAndFeel::getValueTrackColour(false));

    float rectSize = 18.0f;
    for (int i{}; i < pairs.get()->size(); i++)
    {
        if (pairs.get()->at(i).getIsSilenced())
        {
            continue;
        }
        
        // wrap index to circlePoints arrayy, and alternate whether using positive co-ordinates or negative
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
        auto p = circlePoints.at(index);

        // create two rings of dots
        if (i >= halfMaxDots)
        {
            p *= 2;
        }
        else
        {
            p *= 1.2f;
        }
        
        // draw dot for osc 1

        auto size1 = rectSize * pairs.get()->at(i).getNormalisedOsc1Level();
        size1 = jr::Utils::constrainFloat(size1, 0.0f, rectSize);
        auto pos1 = center + p;
        g.fillEllipse(juce::Rectangle<float>(pos1.getX(), pos1.getY(), size1, size1));

        // draw dot for osc 2
        
        auto size2 = rectSize * pairs.get()->at(i).getNormalisedOsc2Level();
        size2 = jr::Utils::constrainFloat(size2, 0.0f, rectSize);
        auto pos2 = center - p;
        g.fillEllipse(juce::Rectangle<float>(pos2.getX(), pos2.getY(), size2, size2));
    }
}