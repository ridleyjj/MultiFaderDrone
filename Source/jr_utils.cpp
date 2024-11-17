/*
  ==============================================================================

    jr_utils.cpp
    Created: 17 Nov 2024 4:17:34pm
    Author:  ridle

  ==============================================================================
*/

#include "jr_utils.h"

float jr::Utils::constrainFloat(float val, float min, float max)
{
	if (val < min)
	{
		val = min;
	}
	else if (val > max)
	{
		val = max;
	}
	return val;
}