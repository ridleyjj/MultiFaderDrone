/*
  ==============================================================================

    jr_juce_utils.h
    Created: 23 Dec 2024 11:55:29am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

namespace jr
{
	/*
	A class with some useful methods that specifically use the JUCE headers
	*/
	class JuceUtils
	{
	public:
		/*
		returns the point on the circumference of a 0,0 centred circle with radius r, at the angle theta.
		Centre point of circle can be optionally passed
		*/
		static juce::Point<float> getPointOnCircle(float r, float theta, juce::Point<float> c = juce::Point<float>(0.0f, 0.0f));
	};
}