/*
  ==============================================================================

    WaveShapeIcon.h
    Created: 19 Dec 2024 7:35:02am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../LookAndFeel/StyleSheet.h"


namespace jr
{
    class WaveShapeIcon : public juce::Component
    {
    public:
        WaveShapeIcon(int _waveShape): waveShape(_waveShape) {}

        void lookAndFeelChanged() override
        {
            if (dynamic_cast<CustomLookAndFeel*>(&(getLookAndFeel())) != nullptr)
            {
                lineColour = dynamic_cast<CustomLookAndFeel*>(&(getLookAndFeel()))->getValueTrackColour(false);
            }
            else
            {
                lineColour = juce::Colours::black;
            }
        }

        void paint(juce::Graphics& g) override
        {
            float x = 0.0f;
            float a = getBounds().getHeight() / 3.0f;
            float offset = a * 1.5f;
            float freq = 20.0f;
            float y = 0.0f;
            float delta = getBounds().getWidth() / (float)numPoints;
            juce::Path wave{};

            auto transform = waveShape == 0 ? sineTransform : triangleTransform;

            juce::Point<float> current = juce::Point<float>{ x, transform(x, a, freq, offset) };

            for (int i{}; i < numPoints; i++)
            {
                x += delta;
                juce::Point<float> next = juce::Point<float>{ x, transform(x, a, freq, offset) };
                wave.addLineSegment(juce::Line<float>(current, next), lineThickness);
                current = next;
            }

            g.setColour(lineColour);
            g.strokePath(wave, juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    private:

        std::function<float(float x, float a, float freq, float offset)> sineTransform = [](float x, float a, float freq, float offset)
            {
                float rad = juce::degreesToRadians(freq * x);
                while (rad > juce::MathConstants<float>::twoPi)
                {
                    rad -= juce::MathConstants<float>::twoPi;
                }
                return a * juce::dsp::FastMathApproximations::sin(rad) + offset;
            };

        std::function<float(float x, float a, float freq, float offset)> triangleTransform = [](float x, float a, float freq, float offset)
            {
                return abs(fmod(2.0f * (x + 0.25f * a), 3 * a) - (1.5f * a)) + (offset * 0.5f);
            };

        int waveShape{ 0 };
        int numPoints{ 50 };
        float lineThickness{ 0.5f };
        juce::Colour lineColour{ juce::Colours::black };
    };
}