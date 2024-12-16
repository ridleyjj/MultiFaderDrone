/*
  ==============================================================================

    jr_RandomOsc.h
    Created: 16 Dec 2024 7:21:31am
    Author:  ridle

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"
#include "../../Utils/jr_Utils.h"

namespace jr
{
    class RandomOsc
    {
    public:
        RandomOsc() {}

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
		Resets the frequency of the oscillator to a new randomised value.
		*/
		void resetOsc();

		/*
		Resets the panning of the oscillator to a new randomised value.
		*/
		void resetPan();

		/*
		Sets the LFO frequency to its new value, using the scale value (between 0 and 1) to set the freq within the lfoSpread and lfoRate
		*/
		float getLfoFreqFromScale(float scale);

		/*
		Processes the level values of the oscillator and returns the current LFO level value to be used to calculate
		the level of output.
		Also checks whether oscillator is currently silenced and resets it's frequencies and panning if so.
		*/
		float processLevels();

		FaderPairs& parent;										// contains shared values such as Frequency Range and Pan Range
		SineOsc lfo;											// LFO to control level of fader					
		SineOsc osc;											// Sine oscillator
		juce::SmoothedValue<float> masterGain{ 0.0f };			// master gain for fading in and out
		bool silenced{ false };
		bool waitingToRestart{ false };							// true if the voice is waiting to reach 0 master gain before restarting
		float lfoBaseFreq{};									// scale value between 0-1 that will be used to set the current LFO rate based on the GUI parameter range set
		std::pair<float, float> out{ 0.5f, 0.5f };				// signal out values left and right
		float pan{ 0.5f };										// pan value for osc, 0=L 1=R 0.5=C
		bool isInitialised{ false };							// false if initialisation is still in progress
		float currentLevel{};									// saved so that level can be sent easily to the GUI
    };
}