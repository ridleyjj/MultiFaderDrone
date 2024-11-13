/*
  ==============================================================================

    StyleSheet.cpp
    Created: 9 Nov 2024 12:48:53pm
    Author:  ridle

  ==============================================================================
*/

#include "StyleSheet.h"

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = roseQuartz;
    auto thumbColour = juce::Colours::lightgrey;
    auto backgroundColour = slider.findColour(juce::Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(5.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    // background
    auto backgroundWidth = (2 * radius) + (lineW * 1.25);
    juce::Point<float> centrePoint(bounds.getCentreX(), bounds.getCentreY());

    g.setColour(backgroundColour);
    g.fillEllipse(juce::Rectangle<float>(backgroundWidth, backgroundWidth).withCentre(centrePoint));

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(fill);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // instead of thumb draw line from centre to arc

    juce::Path dialHead;
    juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
    dialHead.addLineSegment(juce::Line{ centrePoint, thumbPoint }, 0);

    g.setColour(thumbColour);
    g.strokePath(dialHead, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    bool hasZeroMidPoint = slider.getMinimum() < 0.0;

    auto backgroundTrackColour = slider.findColour(juce::Slider::backgroundColourId);
    auto valueTrackColour = roseQuartz;
    auto zeroMarkColour = juce::Colours::lightgrey;

    auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

    juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    juce::Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(backgroundTrackColour);
    g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    juce::Path valueTrack;
    juce::Point<float> minPoint, maxPoint, thumbPoint;

    auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    // draw value track from middle point 0.0 if present
    if (hasZeroMidPoint)
    {
        float unitLength = height / (slider.getMaximum() - slider.getMinimum());
        float zeroPos = slider.isHorizontal() ? sliderPos - (slider.getValue() * unitLength) : sliderPos + (slider.getValue() * unitLength);
        minPoint = {
            slider.isHorizontal() ? zeroPos : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : zeroPos
        };
    }
    else
    {
        minPoint = startPoint;
    }
    
    maxPoint = { kx, ky };

    auto thumbWidth = getSliderThumbRadius(slider);

    valueTrack.startNewSubPath(minPoint);
    valueTrack.lineTo(maxPoint);
    g.setColour(valueTrackColour);
    const auto endStrokeType = hasZeroMidPoint ? juce::PathStrokeType::butt : juce::PathStrokeType::rounded;
    g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, endStrokeType });

    if (hasZeroMidPoint)
    {
        // draw zero line mark
        juce::Path zeroLine;
        
        if (slider.isHorizontal())
        {
            auto markerHalfWidth = height / 4.0f;
            zeroLine.addLineSegment(juce::Line{
                    juce::Point{ minPoint.getX(), minPoint.getY() + markerHalfWidth },
                    juce::Point{ minPoint.getX(), minPoint.getY() - markerHalfWidth },
                }, 0);
        }
        else
        {
            auto markerHalfWidth = width / 4.0f;
            zeroLine.addLineSegment(juce::Line{ 
                    juce::Point{ minPoint.getX() - markerHalfWidth, minPoint.getY() },
                    juce::Point{ minPoint.getX() + markerHalfWidth, minPoint.getY() },
                }, 0);
        }

        g.setColour(zeroMarkColour);
        g.strokePath(zeroLine, juce::PathStrokeType(2.0, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // head marker
    g.setColour(dark);
    g.fillRoundedRectangle(juce::Rectangle<float>(static_cast<float> (thumbWidth * 2.0f), static_cast<float> (thumbWidth)).withCentre(maxPoint), 1.0f);

}