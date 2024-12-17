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

class MultiWaveOsc
{
public:

    void setSampleRate(float _sampleRate)
    {
        tri.setSampleRate(_sampleRate);
        sine.setSampleRate(_sampleRate);
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
        shapeFactor = jr::Utils::constrainFloat(_shape);
    }

    float process()
    {
        float sineOut = sine.process() * (1.0f - shapeFactor);
        float triOut = tri.process() * shapeFactor * triNormaliseFactor;
        return sineOut + triOut;
    }

    float getCurrentFrequency()
    {
        return sine.getCurrentFrequency();
    }

private:
    TriOsc tri{};
    SineOsc sine{};
    float shapeFactor{ 0.0f };
    float triNormaliseFactor{ 0.5f };       // scale factor to make triangle wave closer in volume to Sine
};