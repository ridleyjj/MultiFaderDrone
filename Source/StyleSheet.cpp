/*
  ==============================================================================

    StyleSheet.cpp
    Created: 9 Nov 2024 12:48:53pm
    Author:  ridle

  ==============================================================================
*/

#include "StyleSheet.h"
#include "jr_utils.h"
#include "LockingTwoHeadedSlider.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    setColour(juce::Label::textColourId, juce::Colours::black);
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black.withAlpha(0.0f));
    setColour(juce::ToggleButton::textColourId, juce::Colours::black);
    setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
    setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::black);
    setDefaultSansSerifTypeface(StyleSheet::boldFont);
}

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
    auto isLocked = false;

    // if slider is a LockingTwoHeadedSlider, check whether it is currently locked 
    if (dynamic_cast<jr::LockingTwoHeadedSlider*>(&slider) != nullptr)
    {
        isLocked = dynamic_cast<jr::LockingTwoHeadedSlider*>(&slider)->isLocked();
    }

    auto isTwoVal = (style == juce::Slider::SliderStyle::TwoValueVertical || style == juce::Slider::SliderStyle::TwoValueHorizontal);

    auto backgroundTrackColour = slider.findColour(juce::Slider::backgroundColourId);
    auto valueTrackColour = getValueTrackColour(isLocked);

    

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

    if (isTwoVal) {
        minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

        maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                     slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
    }
    else
    {
        minPoint = startPoint;

        // draw value track to current slider pos
        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

        maxPoint = { kx, ky };
    }

    valueTrack.startNewSubPath(minPoint);
    valueTrack.lineTo(maxPoint);
    g.setColour(valueTrackColour);
    const auto endStrokeType = juce::PathStrokeType::rounded;
    g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, endStrokeType });

    auto thumbWidth = getSliderThumbRadius(slider);

    // head marker(s)

    juce::Rectangle thumbRect = slider.isVertical() ?
        juce::Rectangle<float>(static_cast<float> (thumbWidth * 2.0f), static_cast<float> (thumbWidth)) :
        juce::Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth * 2.0f));

    if (isLocked)
    {
        auto offset = thumbWidth * 0.25f;
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillRoundedRectangle(thumbRect.withCentre(juce::Point(maxPoint.getX(), maxPoint.getY() + offset)), 1.0f);

        if (isTwoVal)
        {
            g.fillRoundedRectangle(thumbRect.withCentre(juce::Point(minPoint.getX(), minPoint.getY() - offset)), 1.0f);
        }
    }
    
    g.setColour(dark);

    g.fillRoundedRectangle(thumbRect.withCentre(maxPoint), 1.0f);

    if (isTwoVal)
    {
        g.fillRoundedRectangle(thumbRect.withCentre(minPoint), 1.0f);
    }
}

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto fontSize = juce::jmin(15.0f, (float)button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 0.9f;

    drawSmallButton(g, button, 4.0f, ((float)button.getHeight() - tickWidth) * 0.5f,
        tickWidth, tickWidth,
        button.getToggleState());

    g.setColour(button.findColour(juce::ToggleButton::textColourId));
    g.setFont(fontSize);

    if (!button.isEnabled())
        g.setOpacity(0.5f);

    g.drawFittedText(button.getButtonText(),
        button.getLocalBounds().withTrimmedLeft(juce::roundToInt(tickWidth) + 10)
        .withTrimmedRight(2),
        juce::Justification::centredLeft, 10);
}


void CustomLookAndFeel::drawSmallButton(juce::Graphics& g, juce::Component& component,
    float x, float y, float w, float h,
    const bool ticked)
{
    juce::Rectangle<float> buttonBounds(x, y, w, h);

    if (ticked)
    {
        g.setColour(verdigris);
        g.fillEllipse(buttonBounds);
    }

    g.setColour(dark);
    g.drawEllipse(buttonBounds, 3.0f);
}

// ============= Extra Methods

juce::Colour CustomLookAndFeel::getVisualiserColour(float brightness)
{
    auto b = 0.25f + (jr::Utils::constrainFloat(brightness) * 0.48f);
    return juce::Colour::fromHSL(0.74f, 0.19f, b, 1.0f);
}