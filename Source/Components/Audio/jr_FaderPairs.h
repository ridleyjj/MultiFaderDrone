#pragma once

#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"
#include "../../Utils/jr_Utils.h"

class FaderPairs
{
public:
	FaderPairs() {}

	/*
	initialises the pairs, creating an array of the max possible size.
	*/
	void init(size_t numPairs, float _sampleRate, size_t maxNumPairs);

	/*
	Processes all of the pairs, and returns the L and R sample out values for all oscs combined.
	*/
	std::pair<float, float> process();

	/*
	Sets the number of desired active pairs and silences / starts voices as needed.
	*/
	void setNumPairs(int numPairs);

	/*
	Sets LFO Rate which effectively controls the range of LFO frequency values. Rate is between 0 and 1
	*/
	void setLfoRate(float _rate);

	/*
	Sets the minimum frequency in Hz that the Oscillators will use
	*/
	void setMinFreq(float _minFreq) { minOscFreq = _minFreq; }

	/*
	Sets the maximum frequency in Hz that the Oscillators will use
	*/
	void setMaxFreq(float _maxFreq) { maxOscFreq = _maxFreq; }

	/*
	* Sets the stereo width of the oscillators. 0.0f = mono, 1.0f = full stereo width.
	*/
	void setStereoWidth(float width);

	// =========================== Nested FaderPair class start ===========================
	// This class is nested so that it can access protected members of the FadersPairs class,
	// allowing these to be shared to avoid unnecessary repetition or memory use

	class FaderPair
	{
	public:
		FaderPair(FaderPairs& _parent) : parent(_parent) {};

		/*
		Initialises oscillators and LFOs with sample rate. Call before playing.
		*/
		void init(float _sampleRate, bool _silenced = false);

		/*
		Sets the sample rate
		*/
		void updateSampleRate(float _sampleRate);

		/*
		Processes the pair of oscillators and returns their next sample out values.
		Call each sample.
		*/
		std::pair<float, float> process();

		/*
		Stops instance.
		*/
		void silence();

		/**
		unsilences a voice - if it is in process of silencing (fading out), will wait until the master reaches 0 before resetting frequencies and volume
		*/
		void start();

		/*
		Triggers instance to recalculate LFO frequency. Use after shared LFO Rate or LFO Spread values have changed.
		*/
		void updateLfoFreq();

		/*
		Returns True if initialisation is finished for instance
		*/
		bool getIsInitialised();

		bool getIsSilenced() { return silenced && masterGain.getCurrentValue() == 0.0f; }

		/*
		Returns the current level of oscillator at given index, normalised to be between 0 and 1
		*/
		float getNormalisedOscLevel(int index)
		{
			if (index == 0)
			{
				return (parent.avgLevel.getCurrentValue() + delta) * parent.normalRatio; // osc 1 current level
			}
			else
			{
				return (parent.avgLevel.getCurrentValue() - delta) * parent.normalRatio; // osc 2 current level
			}
		}

		/*
		Returns the current frequency of the oscillator at the given index. Returns 0.0f if an invalid index is given
		*/
		float getOscFrequency(int oscIndex)
		{
			if (oscIndex < 0 || oscIndex > 2)
			{
				return 0.0f;
			}
			else
			{
				return oscs.at(oscIndex).getCurrentFrequency();
			}
		}

		/*
		Returns the current pan value of the oscillator at the given index. Returns 0.0f if an invalid index is given
		*/
		float getPan(int oscIndex)
		{
			if (oscIndex < 0 || oscIndex > 2)
			{
				return 0.0f;
			}
			else
			{
				return pan.at(oscIndex);
			}
		}

	private:
		/*
		initialises Oscs if they have not yet been created.
		*/
		void initOscs(float _sampleRate);

		/*
		Resets the frequencies of the oscillators and the LFO to new randomised values.
		*/
		void resetFrequencies();

		/*
		Resets the frequency of the oscillator of the given index to a new randomised value.
		*/
		void resetOsc(int index);

		/*
		Resets the panning of the oscillator of the given index to a new randomised value.
		*/
		void resetPan(int index);

		/*
		Sets the LFO frequency to its new value, using the scale value (between 0 and 1) to set the freq within the lfoSpread and lfoRate
		*/
		float getLfoFreqFromScale(float scale);

		/*
		Processes the level values of both oscillators and returns the current LFO level value to be used to calculate
		the level of each osc output.
		Also checks whether either oscillator is currently silenced and resets it's frequencies and panning if so.
		*/
		float processLevels();

		FaderPairs& parent;										// contains shared values such as Frequency Range and Pan Range
		SineOsc lfo;											// LFO to control mix level of faders					
		std::vector<SineOsc > oscs;								// pair of oscillators
		juce::SmoothedValue<float> masterGain{ 0.0f };			// master gain for fading in and out
		bool silenced{ false };
		bool waitingToRestart{ false };							// true if the voice is waiting to reach 0 master gain before restarting
		float lfoBaseFreq{};									// scale value between 0-1 that will be used to set the current LFO rate based on the GUI parameter range set
		std::pair<float, float> out{ 0.5f, 0.5f };				// signal out values left and right
		std::vector<float> pan{ 0.5f, 0.5f };					// array of pan values for oscs, 0=L 1=R 0.5=C
		bool isInitialised{ false };							// false if initialisation is still in progress
		float delta{};											// saved so that levels can be calculated and sent to the GUI
	};
	// =========================== Nested FaderPair class end ===========================

	/*
	Returns a pointer to the vector of oscillator pairs, used so that these can be referenced in the GUI for drawing the sound visualiser 
	*/
	std::shared_ptr<std::vector<FaderPairs::FaderPair>> getPairs() { return std::make_shared <std::vector<FaderPairs::FaderPair>>(_pairs); }

private:
	/*
	Sets gain offset which is used to reduce the apparent loudness difference when increasing the number of voices.
	*/
	void setGainOffset();

	std::vector<FaderPair> _pairs{};
	float sampleRate{};
	int numActivePairs{ 0 };					// how many pairs are currently active i.e. not silenced
	float gainOffset{};							// offset to manage gain difference between few voices and many voices
	juce::SmoothedValue<float> gain{ 0.0f };
	std::pair<float, float> out{};				// signal out values left and right
	bool isInitialised{ false };				// false if initialisation is still in progress

protected:

	/*
	* returns a new randomised Osc Freq value in Hz using the current max and min values.
	*/
	float getRandomOscFrequency() { return (random.nextFloat() * (maxOscFreq - minOscFreq)) + minOscFreq; };

	void processSharedLevels();

	void setMaxLevel(float _maxLevel);

	// variables that are referenced by the list of FaderPair objects
	float rampTime{ 0.2f };
	juce::Random random;						// used for generating random frequency
	float lfoRate{ 0.0f };						// rate to modify the LFO freq by (0-1)
	float minLfoFreq{ 0.01f };					// minimum lfo frequency when generating random in Hz
	float maxLfoFreq{ 5.0f };					// maximum lfo frequency when picking a random frequency in Hz
	float minOscFreq{ 120.0f };					// minimum osc frequency when generating random in Hz
	float maxOscFreq{ 1200.0f };				// maximum osc frequency when picking a random frequency in Hz
	float stereoWidth{ 0.0f };					// pan range 0 - 1.0
	juce::SmoothedValue<float> maxLevel{};		// the maximum combined level of both faders - will be referenced by all oscillator pairs
	juce::SmoothedValue<float> avgLevel{};		// the average level that will serve as a max for each oscillator within a pair so that their combined level does not exceed the max
	float normalRatio{ 1.0f };					// the factor to multiply current osc level by to get level in range of 0-1. Saving to avoid unecessary calculation repetition

};

