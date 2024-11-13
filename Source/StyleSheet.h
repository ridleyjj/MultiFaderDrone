/*
  ==============================================================================

    StyleSheet.h
    Created: 9 Nov 2024 12:48:53pm
    Author:  ridle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FontResources.h"

// fonts
namespace StyleSheet
{

    static juce::Typeface::Ptr boldFont  { juce::Typeface::createSystemTypefaceFor (Resources::WorkSansSemiBold_ttf, Resources::WorkSansSemiBold_ttfSize) };
    static juce::Typeface::Ptr plainFont { juce::Typeface::createSystemTypefaceFor (Resources::WorkSansRegular_ttf, Resources::WorkSansRegular_ttfSize) };

}

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
    public:
        void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos,
            float minSliderPos,
            float maxSliderPos,
            const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    private:
        juce::Colour darkGreen{ juce::Colour(93, 121, 122) };
        juce::Colour greenGray{ juce::Colour(112, 148, 150) };
        juce::Colour verdigris{ juce::Colour(130, 174, 177) };
        juce::Colour skyBlue{ juce::Colour(147, 198, 214) };
        juce::Colour periwinkle{ juce::Colour(167, 172, 217) };
        juce::Colour roseQuartz{ juce::Colour(158, 143, 178) };

        juce::Colour beige{ juce::Colour(217, 197, 178) };

        juce::Colour trackColour{ juce::Colour(38, 50, 56) };
        juce::Colour dark{ juce::Colour(24, 31, 34) };
};

