/*
  ==============================================================================

    StyleSheet.cpp
    Created: 9 Nov 2024 12:48:53pm
    Author:  ridle

  ==============================================================================
*/

#include "StyleSheet.h"
#include "../Utils/jr_utils.h"
#include "../Components/GUI/LockingTwoHeadedSlider.h"
#include "../Components/GUI/DarkModeButton.h"
#include "../Components/GUI/NoValueColourSlider.h"

jr::CustomLookAndFeel::CustomLookAndFeel()
{
    updateTextColour();
    setDefaultSansSerifTypeface(StyleSheet::boldFont);
}

void jr::CustomLookAndFeel::updateTextColour()
{
    setColour(juce::Label::textColourId, getTextColour());
    setColour(juce::Slider::textBoxTextColourId, getTextColour());
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black.withAlpha(0.0f));
    setColour(juce::ToggleButton::textColourId, getTextColour());
    setColour(juce::ToggleButton::tickColourId, getTextColour());
    setColour(juce::ToggleButton::tickDisabledColourId, getTextColour());
}

void jr::CustomLookAndFeel::setIsDarkMode(bool isDarkMode)
{
    darkMode = isDarkMode;
    updateTextColour();
}

void jr::CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = getValueTrackColour(false);
    auto thumbColour = getDialHeadColour();
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

void jr::CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto isLocked = false;
    auto useSeparateValueColour = true;

    // if slider is a LockingTwoHeadedSlider, check whether it is currently locked 
    if (dynamic_cast<jr::LockingTwoHeadedSlider*>(&slider) != nullptr)
    {
        isLocked = dynamic_cast<jr::LockingTwoHeadedSlider*>(&slider)->isLocked();
    }

    if (dynamic_cast<jr::NoValueColourSlider*>(&slider) != nullptr)
    {
        useSeparateValueColour = false;
    }

    auto isTwoVal = (style == juce::Slider::SliderStyle::TwoValueVertical || style == juce::Slider::SliderStyle::TwoValueHorizontal);

    auto backgroundTrackColour = slider.findColour(juce::Slider::backgroundColourId);
    auto valueTrackColour = useSeparateValueColour ? getValueTrackColour(isLocked) : backgroundTrackColour;

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
    
    g.setColour(dark);

    g.fillRoundedRectangle(thumbRect.withCentre(maxPoint), 1.0f);

    if (isTwoVal)
    {
        g.fillRoundedRectangle(thumbRect.withCentre(minPoint), 1.0f);
    }
}

void jr::CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    if (dynamic_cast<jr::DarkModeButton*>(&button) != nullptr)
    {
        drawDarkModeButton(g, button, button.getToggleState(), button.findColour(juce::ToggleButton::textColourId));
        return;
    }

    // standard JUCE LookAndFeel_V4 drawToggleButton method

    auto fontSize = juce::jmin(15.0f, (float)button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 1.1f;

    drawTickBox(g, button, 4.0f, ((float)button.getHeight() - tickWidth) * 0.5f,
        tickWidth, tickWidth,
        button.getToggleState(),
        button.isEnabled(),
        shouldDrawButtonAsHighlighted,
        shouldDrawButtonAsDown);

    g.setColour(button.findColour(juce::ToggleButton::textColourId));
    g.setFont(fontSize);

    if (!button.isEnabled())
        g.setOpacity(0.5f);

    g.drawFittedText(button.getButtonText(),
        button.getLocalBounds().withTrimmedLeft(juce::roundToInt(tickWidth) + 10)
        .withTrimmedRight(2),
        juce::Justification::centredLeft, 10);
}


void jr::CustomLookAndFeel::drawDarkModeButton(juce::Graphics& g, juce::Component& component, const bool ticked, juce::Colour colour)
{
    auto size = juce::jmin(component.getBounds().getWidth(), component.getBounds().getHeight()) * 0.5f;
    auto centre = component.getLocalBounds().getCentre().toFloat();

    ticked ? drawSun(g, size, centre, colour) : drawCrescent(g, size, centre, colour);
}

void jr::CustomLookAndFeel::drawCrescent(juce::Graphics& g, float size, juce::Point<float> c, juce::Colour colour)
{
    juce::Path crescent;
    
    auto r = size * 0.5f;

    // large outside arc
    crescent.addCentredArc(c.getX(),
        c.getY(),
        r,
        r,
        0.0f,
        4.08407f,
        0.0f,
        true);

    // small inside arc
    float innerR = r * 0.9f;
    crescent.addCentredArc(c.getX() - r / 2,
        c.getY() - r / 5,
        innerR,
        innerR,
        0.0f,
        3.45575f,
        0.62831f,
        true);

    g.setColour(colour);
    g.strokePath(crescent, juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void jr::CustomLookAndFeel::drawSun(juce::Graphics& g, float size, juce::Point<float> c, juce::Colour colour)
{
    g.setColour(colour);

    // draw centre circle
    auto circleSize = size * 0.5f;
    g.drawEllipse(juce::Rectangle<float>(circleSize, circleSize).withCentre(c), 1.0f);

    // draw sun rays
    auto angle = 0.0f;
    juce::Path rays;
    int numRays = 9;
    float rayStart = size * 0.4f;
    float rayEnd = size * 0.5f;
    for (int i{}; i < numRays; i++)
    {
        rays.addLineSegment(juce::Line<float>(getPointOnCircle(rayStart, angle, c), getPointOnCircle(rayEnd, angle, c)), 1.0f);
        angle += juce::MathConstants<float>::twoPi / numRays;
    }
    g.strokePath(rays, juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

juce::Point<float> jr::CustomLookAndFeel::getPointOnCircle(float r, float theta, juce::Point<float> c)
{
    return juce::Point<float>(
        c.getX() + r * juce::dsp::FastMathApproximations::cos(theta),
        c.getY() + r * juce::dsp::FastMathApproximations::sin(theta)
    );
}

// ============= Extra Methods

juce::Colour jr::CustomLookAndFeel::getVisualiserColour(float brightness)
{
    auto b = 0.35f + (jr::Utils::constrainFloat(brightness) * 0.38f);
    if (darkMode)
    {
        b = jr::Utils::constrainFloat(b + 0.2f);
    }
    return juce::Colour::fromHSL(0.74f, 0.19f, b, 1.0f);
}