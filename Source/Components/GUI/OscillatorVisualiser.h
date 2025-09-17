/*
  ==============================================================================

    OscillatorVisualiser.h
    Created: 25 Nov 2024 3:39:49pm
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>
#include "../Audio/jr_FaderPairs.h"
#include "../../LookAndFeel/StyleSheet.h"

namespace jr
{
    /*
    The visualiser component that will use an array of FaderPair objects to draw circles which represent each oscillator
    */
    class OscillatorVisualiser : public juce::Component
    {
    public:
        OscillatorVisualiser(jr::CustomLookAndFeel& _lookAndFeel) : lookAndFeel(_lookAndFeel) {}

        void paint(juce::Graphics&) override;
        
        /*
        Sets the FaderPairs that will be referenced whilst painting the visualiser
        */
        void setPairs(std::shared_ptr<std::vector<FaderPairs::RandomOsc>> _pairs) { oscs = _pairs; }

        /*
        Sets how many oscillators are currently active in order to limit the loops in paint. Saved as a variable in order to save
        time from calculation every paint call
        */
        void setNumActivePairs(int _numActivePairs) { numActivePairs = _numActivePairs; }

    private:

        void resized() override;

        /*
        Returns a new Point that takes the given point p and adds random noise to both co-ordinates. Scale is 2.0f by default which results in a noise range of +/- 1.0f
        */
        juce::Point<float> addRandomNoiseToPoint(juce::Point<float> p, float scale = 2.0f);

        /*
        Uses index to return an evenly spaced point on a zero centred circle's circumference
        */
        juce::Point<float> getCircumferencePoint(int i);

        /*
        Returns a colour based on the frequency of the oscillator at the given index in the given FaderPair
        */
        juce::Colour getColourFromOsc(FaderPairs::RandomOsc& osc);

        /*
        Returns the size of dot to draw based on the current level of the given oscillator at the given index in the FaderPair.
        Size will be constrained between the max Dot size and 0
        */
        float getDotSizeFromOsc(FaderPairs::RandomOsc& osc);

        /*
        Returns the point to draw the dot at for oscillator at the given index in the FaderPair.
        circumferencePoint is the point on a circle's circumference (with centre 0 and radius 66)
        marking the angle at which the dot should be drawn.
        */
        const juce::Point<float> getPointFromOsc(FaderPairs::RandomOsc& osc, bool direction, const juce::Point<float>& circumferencePoint);

        /*
        Draws a dot representation of the oscillator at the given index in the FaderPair, using
        circumferencePoint as the point on a 0 centred circle that matches the angle of the desired dot.
        */
        void drawDotForOsc(juce::Graphics& g, FaderPairs::RandomOsc& osc, bool direction, const juce::Point<float>& circumferencePoint);

        /*
        Draws a dot with additional overlayed dots with random noise to cause a dynamic 'buzzing'/'wobbling' effect. Draws a
        dot with given size at point p.
        */
        void drawWobble(juce::Graphics& g, const juce::Point<float>& p, float size);
        
        /*
        Draws spikes around the dot, spike size will be relative to waveshape of osc
        */
        void drawSpikes(juce::Graphics& g, const juce::Point<float>& p, float size, FaderPairs::RandomOsc& osc);

        /*
        draws an equilateral triangle with centre point c, of size that fits in circle of radius r.
        offset will rotate the triangle. Offset wraps 0.0f-(twopi / 3)
        */
        void drawTriangle(juce::Graphics& g, float r, const juce::Point<float>& c, float offset = 0.0f);

        jr::CustomLookAndFeel& lookAndFeel;
        float maxDotSize{ 28.0f };                                              // maximum size for a dot in the visualiser (the size the dot will be at max volume)
        float maxRadius{ 4.0f };                                                // scale value that determines how far out the visualiser will spread out.
        float minRadius{ 0.2f };                                                // scale value that determines how clustered the visualiser will be when mono.
        int numActivePairs{ 0 };                                                // number of oscs currently playing - saved to avoid unnecessary calculation or loops
        float minFreq = 80.0f;                                                  // min Frequency (in Hz) used for colour grading
        float maxFreq = 1700.0f;                                                // max Frequency (in Hz) used for colour grading
        std::vector<juce::Point<float>> circlePoints                            // approximate points on a semi-circle with centre point 0,0 and radius of 66. Used to save calc costs
        {
            juce::Point<float>(66.0f, 0.0f),
            juce::Point<float>(0.0f, 66.0f),
            juce::Point<float>(46.5f, -46.5f),
            juce::Point<float>(46.5f, 46.5f),
            juce::Point<float>(60.2f, -27.0f),
            juce::Point<float>(27.0f, 60.2f),
            juce::Point<float>(27.0f, -60.2f),
            juce::Point<float>(60.2f, 27.0f),
        };
        std::shared_ptr<std::vector<FaderPairs::RandomOsc>> oscs{ nullptr };   // pointer to the array of FaderPair objects that will be visualised
        juce::Random random{};
        juce::Point<float> relativeCentre{ 0.0f, 0.0f };                        // centre of visualiser relative to its own top left corner, saved on resize to avoid unnecessary repeated conversions

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorVisualiser);
    };
}
