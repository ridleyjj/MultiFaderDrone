/*
  ==============================================================================

    jr_utils.h
    Created: 17 Nov 2024 4:17:34pm
    Author:  ridle

  ==============================================================================
*/

#pragma once

namespace jr
{
	class Utils
	{
	public:
		/*
		returns the value of the float provided, constrained by the min and max values given. By default constrains between 0 and 1
		*/
		static float constrainFloat(float val, float min = 0.0f, float max = 1.0f);
	};
}