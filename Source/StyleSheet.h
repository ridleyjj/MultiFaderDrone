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

        CustomLookAndFeel();

        void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos,
            float minSliderPos,
            float maxSliderPos,
            const juce::Slider::SliderStyle style, juce::Slider& slider) override;

        static juce::Colour getValueTrackColour(bool isFrozen) { return isFrozen ? verdigris : roseQuartz; }

        static juce::Colour getBackgroundColour() { return beige; }

    private:
        static inline juce::Colour verdigris{ juce::Colour(130, 174, 177) };
        static inline juce::Colour roseQuartz{ juce::Colour(158, 143, 178) };
        static inline juce::Colour beige{ juce::Colour(240, 227, 214) };
        static inline juce::Colour trackColour{ juce::Colour(38, 50, 56) };
        static inline juce::Colour dark{ juce::Colour(24, 31, 34) };

        // currently unused
        static inline juce::Colour skyBlue{ juce::Colour(147, 198, 214) };
        static inline juce::Colour periwinkle{ juce::Colour(167, 172, 217) };
        static inline juce::Colour darkGreen{ juce::Colour(93, 121, 122) };
        static inline juce::Colour greenGray{ juce::Colour(112, 148, 150) };
};

