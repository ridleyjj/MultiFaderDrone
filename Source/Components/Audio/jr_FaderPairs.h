#pragma once

#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"
#include "jr_MultiWaveOsc.h"
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
	Sets the number of desired active oscs and silences / starts voices as needed.
	*/
	void setNumOscs(int numOscs);

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

	/*
	Sets the wave shape of the oscillators 0=Sine 1=Triange, between those values mixes the two shapes proportionally
	*/
	void setWaveShape(float _waveShape);

	// =========================== Nested RandomOsc class start ===========================
	// This class is nested so that it can access protected members of the FadersPairs class,
	// allowing these to be shared to avoid unnecessary repetition or memory use

	class RandomOsc
	{
	public:
		RandomOsc(FaderPairs& _parent) : parent(_parent) {};

		/*
		Initialises oscillators and LFOs with sample rate. Call before playing.
		*/
		void init(float _sampleRate, bool _silenced = false);

		/*
		Sets the sample rate
		*/
		void updateSampleRate(float _sampleRate);

		/*
		Processes the oscillator and returns the next sample out values.
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
		Returns the current level of oscillator, normalised to be between 0 and 1
		*/
		float getNormalisedOscLevel()
		{
			return currentLevel * parent.normalRatio;
		}

		/*
		Returns the current frequency of the oscillator at the given index
		*/
		float getOscFrequency()
		{
			return osc.getCurrentFrequency();
		}

		/*
		Returns the current pan value of the oscillator
		*/
		float getPan()
		{
			return pan;
		}

		void setWaveShape(float _waveShape) { osc.setWaveShape(_waveShape); }

	private:
		/*
		initialises Osc if it has not yet been created.
		*/
		void initOsc(float _sampleRate);

		/*
		Resets the frequencies of the oscillators and the LFO to new randomised values.
		*/
		void resetFrequencies();

		/*
		Resets the frequency of the oscillator to a new randomised value.
		*/
		void resetOsc();

		/*
		Resets the panning of the oscillator to a new randomised value.
		*/
		void resetPan();

		/*
		Resets the waveshape of the oscillator to a new randomised value.
		*/
		void resetShape();

		/*
		Sets the LFO frequency to its new value, using the scale value (between 0 and 1) to set the freq within the lfoSpread and lfoRate
		*/
		float getLfoFreqFromScale(float scale);

		/*
		Processes the level values of the oscillator and returns the current LFO level value to be used to calculate
		the level of output.
		Also checks whether oscillator is currently silenced and resets it's frequencies and panning if so.
		*/
		float processLevel();

		FaderPairs& parent;										// contains shared values such as Frequency Range and Pan Range
		SineOsc lfo;											// LFO to control level of fader					
		jr::MultiWaveOsc osc;										// Audible oscillator
		juce::SmoothedValue<float> masterGain{ 0.0f };			// master gain for fading in and out
		bool silenced{ false };
		bool waitingToRestart{ false };							// true if the voice is waiting to reach 0 master gain before restarting
		float lfoBaseFreq{};									// scale value between 0-1 that will be used to set the current LFO rate based on the GUI parameter range set
		std::pair<float, float> out{ 0.5f, 0.5f };				// signal out values left and right
		float pan{ 0.5f };										// pan value for osc, 0=L 1=R 0.5=C
		bool isInitialised{ false };							// false if initialisation is still in progress
		float currentLevel{};									// saved so that level can be sent easily to the GUI
	};
	// =========================== Nested RandomOsc class end ===========================

	/*
	Returns a pointer to the vector of oscillators, used so that these can be referenced in the GUI for drawing the sound visualiser 
	*/
	std::shared_ptr<std::vector<FaderPairs::RandomOsc>> getOscs() { return std::make_shared <std::vector<FaderPairs::RandomOsc>>(_oscs); }

private:
	/*
	Sets gain offset which is used to reduce the apparent loudness difference when increasing the number of voices.
	*/
	void setGainOffset();

	std::vector<RandomOsc> _oscs{};
	float sampleRate{};
	int numActiveOscs{ 0 };						// how many oscs are currently active i.e. not silenced
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

	// variables that are referenced by the list of RandomOsc objects
	float rampTime{ 0.05f };
	juce::Random random;						// used for generating random frequency
	float lfoRate{ 0.0f };						// rate to modify the LFO freq by (0-1)
	float minLfoFreq{ 0.01f };					// minimum lfo frequency when generating random in Hz
	float maxLfoFreq{ 5.0f };					// maximum lfo frequency when picking a random frequency in Hz
	float minOscFreq{ 120.0f };					// minimum osc frequency when generating random in Hz
	float maxOscFreq{ 1200.0f };				// maximum osc frequency when picking a random frequency in Hz
	float stereoWidth{ 0.0f };					// pan range 0 - 1.0
	juce::SmoothedValue<float> maxLevel{};		// the maximum combined level of each osc fader - will be referenced by all oscillators
	float normalRatio{ 1.0f };					// the factor to multiply current osc level by to get level in range of 0-1. Saving to avoid unecessary calculation repetition
	float waveShape{ 0.0f };					// waveShape to be used by oscialltors, 0=Sine, 1=Tri

};

