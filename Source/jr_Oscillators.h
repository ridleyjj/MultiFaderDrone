#pragma once

#define pi 3.14159
#include <iostream>
#include <cmath>		// used for sin() and fabs()
#include <JuceHeader.h> // used for SmoothedValue

/// <summary>
/// A simple phasor - use setSampleRate() [default set to 44100Hz] and setFrequency() before calling process()
/// </summary>
class Phasor
{
public:

	/**
	* sets the sample rate in Hz
	*/
	void setSampleRate(float _sampleRate)
	{
		sampleRate = _sampleRate;
		frequency.reset(sampleRate, rampTimeInSeconds);
	}

	/**
	* sets the frequency rate in Hz and updates the phase delta
	*/
	void setFrequency(float _frequency)
	{
		frequency.setCurrentAndTargetValue(_frequency);
	}

	void setFrequencyOverTime(float _frequency) {
		frequency.setTargetValue(_frequency);
	}

	/**
	*  increments the phase and returns the next sample value
	*/
	float process()
	{
		phaseDelta = frequency.getNextValue() / (float)sampleRate;
		phase += phaseDelta;

		if (phase >= 1)
			phase -= 1.0f;

		return output(phase);
	}

	/**
	* specific output process for different types of oscillator
	*/
	virtual float output(float _phase)
	{
		return _phase;
	}

	void setRampTime(float _rampTimeInSeconds) {
		if (_rampTimeInSeconds < 0.01f) {
			_rampTimeInSeconds = 0.01f;
		}
		rampTimeInSeconds = _rampTimeInSeconds;
	}

	float getCurrentFrequency() {
		return frequency.getCurrentValue();
	}

private:
	float sampleRate = 44100;
	juce::SmoothedValue<float> frequency;
	float phase = 0;
	float phaseDelta;
	float rampTimeInSeconds{ 2.0f };
};

/// <summary>
/// Sine Oscillator - use setSampleRate() [default set to 44100Hz] and setFrequency() before calling process() 
/// </summary>
class SineOsc : public Phasor
{
	/**
	* returns the next sample value
	*/
	float output(float _phase) override
	{
		float sample = std::sin(2.0 * pi * _phase);

		return sample;
	}
};

/// <summary>
/// Triangle Oscillator - use setSampleRate() [default set to 44100Hz] and setFrequency() before calling process() 
/// </summary>
class TriOsc : public Phasor
{
	/**
	* returns the next sample value
	*/
	float output(float _phase) override
	{
		float sample = (fabs((_phase - 0.5)) - 0.25) * 4;

		return sample;
	}
};

/// <summary>
/// Square Oscillator - use setSampleRate() [default set to 44100Hz] and setFrequency() before calling process(), pulse width can be set with setPulseWidth() [default 0.5]
/// </summary>
class SquareOsc : public Phasor
{
public:

	/**
	* sets the pulseWidth of the square wave (0 - 1) - default is 0.5
	*/
	void setPulseWidth(float _pulseWidth)
	{
		pulseWidth = _pulseWidth;
	}

private:
	/**
	* returns the next sample value
	*/
	float output(float _phase) override
	{
		float sample = 1;

		if (_phase > pulseWidth)
			sample = -1;

		return sample;
	}

	float pulseWidth = 0.5;
};