/*
  ==============================================================================

    jr_NormalisedOscs.h
    Created: 17 Dec 2024 7:32:13am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include "jr_Oscillators.h"
#include "../../Utils/jr_Utils.h"


namespace jr
{
    class MultiWaveOsc
    {
    public:

        void setSampleRate(float _sampleRate)
        {
            tri.setSampleRate(_sampleRate);
            sine.setSampleRate(_sampleRate);

            shapeFactor.reset(_sampleRate, 0.1f);
        }
    
        void setFrequency(float _frequency)
        {
                tri.setFrequency(_frequency);
                sine.setFrequency(_frequency);
        }

        /*
        Sets the wave shape of the osc 0=Sine 1=Triangle
        Any value between 0 and 1 will mix the outputs of both shapes proportional to the value
        */
        void setWaveShape(float _shape)
        {
            shapeFactor.setTargetValue(jr::Utils::constrainFloat(_shape));
        }

        float process()
        {
            shapeFactor.getNextValue();
            float sineOut = sine.process() * (1.0f - shapeFactor.getCurrentValue());
            float triOut = tri.process() * shapeFactor.getCurrentValue();
            return sineOut + triOut;
        }

        float getCurrentFrequency()
        {
            return sine.getCurrentFrequency();
        }

        float getWaveShape() { return shapeFactor.getCurrentValue(); }

    private:
        TriOsc tri{};
        SineOsc sine{};
        juce::SmoothedValue<float> shapeFactor{ 0.0f };
    };
}