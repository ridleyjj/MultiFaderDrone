#pragma once

#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"

namespace jr 
{
	class Utils
	{
	public:
		/*
		* returns the value of the float provided, constrained by the min and max values given. By default constrains between 0 and 1
		*/
		static float constrainFloat(float val, float min = 0.0f, float max = 1.0f)
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
	};
}

class FaderPair
{
public:
	FaderPair();

	void init(float _sampleRate, float _maxLevel = 1.0f, bool _silenced = false);

	/*
	* Sets min and max frequencies for Oscs and LFOs for all FaderPair instances. Required to call before creating
	* instances. By default sets min and max LFO frequencies to 0.01 and 0.1 if none passed.
	* All values in Hz.
	*/
	static void initFreqs(float _minOscFreq, float _maxOscFreq, float _minLfoFreq = 0.01f, float _maxLfoFreq = 0.1f);

	/*
	* Processes the pair of oscillators and returns their next sample out values.
	* Call each sample.
	*/
	std::pair<float, float> process();

	/*
	* Stops instance.
	*/
	void silence();

	/**
	* unsilences a voice - if it is in process of silencing (fading out), will wait until the master reaches 0 before resetting frequencies and volume
	*/
	void start();

	/*
	* Sets the max output level that the Pair is allowed to reach.
	*/
	void setMaxLevel(float _maxLevel);

	/*
	* Triggers instance to recalculate LFO frequency. Use after static LFO Rate or LFO Spread values have changed.
	*/
	void updateLfoFreq();

	/*
	* Sets Stereo Width for all instances. 0.0f = mono, 1.0f = full stereo spread.
	*/
	static void setStereoWidth(float width);

	/*
	* Sets LFO Rate for all instances. Rate is between 0.0f and 1.0f, and the higher it is the greater the spread of possible
	* LFO frequencies, and the higher the max LFO freq is. Min LFO Freq stays constant.
	*/
	static void setLfoRate(float _rate);

private:
	/*
	* initialises Oscs if they have not yet been created.
	*/
	void initOscs(float _sampleRate);

	/*
	* Resets the frequencies of the oscillators and the LFO to new randomised values.
	*/
	void resetFrequencies();

	/*
	* Resets the frequency of the oscillator of the given index to a new randomised value.
	*/
	void resetOsc(int index);

	/*
	* Resets the panning of the oscillator of the given index to a new randomised value.
	*/
	void resetPan(int index);

	/*
	* Sets the LFO frequency to its new value, using the scale value (between 0 and 1) to set the freq within the lfoSpread and lfoRate
	*/
	float getLfoFreqFromScale(float scale);

	/*
	* Processes the level values of both oscillators and returns the current LFO level value to be used to calculate
	* the level of each osc output.
	* Also checks whether either oscillator is currently silenced and resets it's frequencies and panning if so.
	*/
	float processLevels();

	SineOsc lfo;											// LFO to control mix level of faders
	juce::SmoothedValue<float> maxLevel;					// the maximum combined level of both faders
	juce::SmoothedValue<float> avgLevel;						
	std::vector<SineOsc > oscs;
	float rampTime{ 0.05f };									// time in seconds for fading in and out
	juce::SmoothedValue<float> masterGain{ 0.0f };			// master gain for fading in and out
	bool silenced{ false };
	bool waitingToRestart{ false };							// true if the voice is waiting to reach 0 master gain before restarting
	float lfoBaseFreq{};
	std::pair<float, float> out{ 0.5f, 0.5f };
	std::vector<float> pan{ 0.5f, 0.5f };					// array of pan values for oscs, 0=L 1=R 0.5=C
	
	// shared static variables and methods

	static inline juce::Random random;						// used for generating random frequency
	static inline float lfoRate{ 0.0f };					// rate to modify the LFO freq by (0-1)
	static inline float lfoSpread{ 1.0f };
	static inline float minLfoFreq;							// minimum lfo frequency when generating random in Hz
	static inline float maxLfoFreq;							// range when picking a random frequency in Hz
	static inline float minOscFreq;							// minimum lfo frequency when generating random in Hz
	static inline float maxOscFreq;							// range when picking a random frequency in Hz
	static inline float stereoWidth{ 0.0f };				// pan range 0 - 1.0

	/*
	* returns a new randomised Osc Freq value in Hz using the current max and min values.
	*/
	static float getRandomOscFrequency();
};

class FaderPairs
{
public:
	FaderPairs() {};

	/*
	* initialises the pairs, creating an array of the max possible size.
	*/
	void init(size_t numPairs, float _sampleRate, size_t maxNumPairs, float minOscFreq = 120.0f, float maxOscFreq = 1200.0f);

	/*
	* Processes all of the pairs, and returns the L and R sample out values for all oscs combined.
	*/
	std::pair<float, float> process();

	/*
	* Sets the number of desired active pairs and silences / starts voices as needed.
	*/
	void setNumPairs(size_t numPairs);

	/*
	* Sets LFO Rate which effectively controls the range of LFO frequency values. Rate is between 0 and 1
	*/
	void setLfoRate(float _rate);

	/*
	* Sets min and max frequency values in Hz for the oscillators.
	*/
	void setOscFreqRange(float _minHz, float _maxHz);

	/*
	* Sets the stereo width of the oscillators. 0.0f = mono, 1.0f = full stereo width.
	*/
	void setStereoWidth(float width);

private:
	/*
	* Sets gain offset which is used to reduce the apparent loudness difference when increasing the number of voices.
	*/
	void setGainOffset();

	std::vector<FaderPair> pairs;
	float sampleRate;
	size_t numActivePairs;
	size_t maxNumPairs;
	float gainOffset;							// offset to manage gain difference between few voices and many voices
	juce::SmoothedValue<float> gain{ 0.0f };
	std::pair<float, float> out{};
};

