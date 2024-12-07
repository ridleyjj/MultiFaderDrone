/*
  ==============================================================================

    StyleSheet.h
    Created: 9 Nov 2024 12:48:53pm
    Author:  ridle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Resources/FontResources.h"

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

        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void drawSmallButton(juce::Graphics& g, juce::Component& component,
            float x, float y, float w, float h,
            const bool ticked, juce::Colour colour);

        static juce::Colour getValueTrackColour(bool isFrozen) { return isFrozen ? getFrozenColour() : getAccentColour(); }

        static juce::Colour getBackgroundColour() { return darkMode ? jet : beige; }

        /*
        Returns a colour with the brightness adjusted based on the given brightness value. Brightness should be a float between 0.0f and 1.0f
        */
        static juce::Colour getVisualiserColour(float brightness);

        void setIsDarkMode(bool isDarkMode);

        static juce::Colour getTextColour() { return darkMode ? juce::Colours::white : juce::Colours::black; }
        
        static juce::Colour getDialHeadColour() { return darkMode ? juce::Colours::white : juce::Colours::lightgrey; }
        
        static juce::Colour getSliderBackgroundColour() { return dark; }

        static juce::Colour getSliderHeadColour() { return dark; }

    private:
        static void _setIsDarkMode(bool isDarkMode) { CustomLookAndFeel::darkMode = isDarkMode; }

        void updateTextColour();

        static juce::Colour getAccentColour() { return darkMode ? lightRose : roseQuartz; }
        static juce::Colour getFrozenColour() { return darkMode ? lightBlue : verdigris; }

        static inline bool darkMode{ false };

        static inline juce::Colour verdigris{ juce::Colour(104, 149, 161) };
        static inline juce::Colour roseQuartz{ juce::Colour(150, 134, 172) };
        static inline juce::Colour beige{ juce::Colour(249, 245, 241) };
        static inline juce::Colour dark{ juce::Colour(24, 31, 34) };
        
        // dark mode
        static inline juce::Colour jet{ juce::Colour(71, 77, 91) };
        static inline juce::Colour lightRose{ juce::Colour(188, 175, 212) };
        static inline juce::Colour lightBlue{ juce::Colour(141, 185, 212) };
};

