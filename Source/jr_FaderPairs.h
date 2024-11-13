#pragma once

#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"

class FaderPair
{
public:
	FaderPair() {};

	void init(float _sampleRate, float _minOscFreq, float _maxOscFreq, float _maxLevel = 1.0f, bool _silenced = false) {
		minOscFreq = _minOscFreq;
		maxOscFreq = _maxOscFreq;

		silenced = _silenced;

		masterGain.reset(_sampleRate, rampTime);
		masterGain.setCurrentAndTargetValue(0.0f);
		masterGain.setTargetValue(silenced ? 0.0f : 1.0f);
		avgLevel.reset(_sampleRate, rampTime);
		maxLevel.reset(_sampleRate, rampTime);

		lfo.setSampleRate(_sampleRate);
		maxLevel.setTargetValue(_maxLevel);
		avgLevel.setTargetValue(_maxLevel / 2.0f);

		lfoBaseFreq = getRandomLfoFrequency();
		lfo.setFrequency(lfoBaseFreq);

		initOscs(_sampleRate);
	}

	float process() {

		if (masterGain.getCurrentValue() == 0.0f && waitingToRestart) {
			waitingToRestart = false;
			start();
		}

		// process next sampleOut
		float delta = processLevels(); // avgLevel gets incremented in here
		float sampleOut{};
		sampleOut += oscs.at(0).process() * (avgLevel.getCurrentValue() + delta);
		sampleOut += oscs.at(1).process() * (avgLevel.getCurrentValue() - delta);
		return sampleOut * masterGain.getNextValue();
	}

	void silence() {
		if (!silenced) {
			masterGain.setTargetValue(0);
			silenced = true;
		}
		waitingToRestart = false;
	}

	/**
	* unsilences a voice - if it has already been silenced, will wait until the master reaches 0 before resetting frequencies and volume
	*/
	void start() {
		if (masterGain.getCurrentValue() != 0.0f && silenced) {
			// if the voice is silenced but has not yet been faded completely, wait
			waitingToRestart = true;
		}
		else if (masterGain.getCurrentValue() == 0.0f) {
			// if voice is already silenced reset it and start
			resetFrequencies();
			masterGain.setTargetValue(1.0f);
			silenced = false;
		}
		// if voice is not currently silenced do nothing
	}

	void restart() {
		waitingToRestart = true;
		masterGain.setTargetValue(0.0f);
	}

	bool isSilenced() {
		return silenced && !waitingToRestart;
	}

	void setMaxLevel(float _maxLevel) {
		maxLevel.setTargetValue(_maxLevel);
		avgLevel.setTargetValue(_maxLevel / 2.0f);
	}

	void setLfoRate(float _rate) {
		if (_rate < 0.0f) {
			_rate = 0.0f;
		}
		else if (_rate > 1.0f) {
			_rate = 1.0f;
		}
		lfoRate = _rate;
		lfo.setFrequency(lfoBaseFreq + (lfoSpread * lfoRate));
	}

	void spreadFreq(float spread, float medianFreq) {
		// spread is between -0.1 and 0.1
		// new freq will be currentFreq + (medianFreq - currentFreq) * spread
		for (auto& osc : oscs) {
			osc.setFrequencyOverTime(osc.getCurrentFrequency() + ((medianFreq - osc.getCurrentFrequency()) * spread));
		}
	}

	// TODO: no need to store min and max for each pair, instead save these in parent
	void setOscFreqRange(float minHz, float maxHz) {
		minOscFreq = minHz;
		maxOscFreq = maxHz;
	}

private:
	float getRandomLfoFrequency() {
		return (random.nextFloat() * (maxLfoFreq - minLfoFreq)) + minLfoFreq;
	}
	
	float getRandomOscFrequency() {
		return (random.nextFloat() * (maxOscFreq - minOscFreq)) + minOscFreq;
	}

	float setAndConstrain(float newValue) {
		if (newValue > maxLevel.getCurrentValue()) {
			return maxLevel.getCurrentValue();
		}
		else if (newValue < 0.0f) {
			return 0.0f;
		}
		return newValue;
	}

	void initOscs(float _sampleRate) {
		for (int i{}; i < 2; i++) {
			SineOsc osc = SineOsc();
			osc.setSampleRate(_sampleRate);
			osc.setFrequency(getRandomOscFrequency());
			oscs.push_back(osc);
		}
	}

	void resetFrequencies() {
		for (auto& osc : oscs) {
			osc.setFrequency(getRandomOscFrequency());
		}
		lfo.setFrequency(getRandomLfoFrequency());
	}

	void resetOsc(int index) {
		if (index < 0 || index >= oscs.size()) {
			return;
		}
		oscs.at(index).setFrequency(getRandomOscFrequency());
	}

	float processLevels() {
		maxLevel.getNextValue();
		float lfoVal = lfo.process();
		if (lfoVal >= 1.0f) {
			resetOsc(1);
		}
		else if (lfoVal <= -1.0f) {
			resetOsc(0);
		}
		return lfoVal * avgLevel.getNextValue();
	}

	SineOsc lfo;											// LFO to control mix level of faders
	juce::SmoothedValue<float> maxLevel;											// the maximum combined level of both faders
	juce::SmoothedValue<float> avgLevel;						
	juce::Random random;									// used for generating random frequency
	float minLfoFreq{ 0.001f };								// minimum lfo frequency when generating random in Hz
	float maxLfoFreq{ 0.1f };								// range when picking a random frequency in Hz
	std::vector<SineOsc > oscs;
	float minOscFreq{ 120.0f };								// minimum lfo frequency when generating random in Hz
	float maxOscFreq{ 1200.0f };							// range when picking a random frequency in Hz
	float rampTime{ 0.1f };									// time in seconds for fading in and out
	juce::SmoothedValue<float> masterGain{ 0.0f };			// master gain for fading in and out
	bool silenced{ false };
	bool waitingToRestart{ false };							// true if the voice is waiting to reach 0 master gain before restarting
	float lfoRate{ 0.0f };									// rate to modify the LFO freq by (0-1)
	float lfoBaseFreq{};
	float lfoSpread{ 1.0f };
};

class FaderPairs
{
public:
	FaderPairs() {};

	void init(size_t numPairs, float _sampleRate, size_t maxNumPairs, float minFreq = 120.0f, float maxFreq = 1200.0f) {
		sampleRate = _sampleRate;
		gain.reset(sampleRate, 0.1f);

		medianFreq = minFreq + (maxFreq - minFreq) * 0.5f;


		const float maxLevel = 0.5f / (float)numPairs;
		for (int i{}; i < maxNumPairs; i++) {
			pairs.push_back(FaderPair());
			pairs.at(i).init(sampleRate, minFreq, maxFreq, maxLevel, i >= numPairs);
		}
		numActivePairs = numPairs;
		setGainOffset();
	}

	float process() {
		float sampleOut{};
		for (auto& pair : pairs) {
			sampleOut += pair.process();
		}
		return sampleOut * gain.getNextValue();
	}

	void setNumPairs(size_t numPairs) {
		if (numPairs < 0) {
			numPairs = 0;
		}
		else if (numPairs > maxNumPairs) {
			numPairs = maxNumPairs;
		}
		float maxLevel = 1.0f / (float)numPairs;
		if (numPairs < pairs.size()) {
			for (int i{}; i < pairs.size(); i++) {
				if (i < numPairs) {
					pairs.at(i).start();
				}
				else {
					pairs.at(i).silence();
				}
				pairs.at(i).setMaxLevel(maxLevel);
			}
		}

		numActivePairs = numPairs;
		setGainOffset();
	}

	void setLfoRate(float _rate) {
		for (auto& pair : pairs) {
			pair.setLfoRate(_rate);
		}
	}

	void setSpread(float spread) {
		if (spread < -1.0f) {
			spread = -1.0f;
		}
		else if (spread > 1.0f) {
			spread = 1.0f;
		}
		for (auto& pair : pairs) {
			pair.spreadFreq(spread, medianFreq);
		}
	}

	void reset() {
		for (int i{}; i < numActivePairs; i++) {
			pairs.at(i).restart();
		}
	}

	void setOscFreqRange(float minHz, float maxHz) {
		minHz = setAndConstrain(minHz, 120.0f, 2000.0f);
		maxHz = setAndConstrain(maxHz, 120.0f, 2000.0f);
		for (auto& pair : pairs)
		{
			pair.setOscFreqRange(minHz, maxHz);
		}
	}

private:
	void setGainOffset() {
		gainOffset = (numActivePairs - 1) / 7.0f;
		if (gainOffset > 1.0f) {
			gainOffset = 1.0f;
		}

		// gain offset is now 0.0f if numActiveParis is 2, or 1.0f if numActivePairs is 14 or greater
		gain.setTargetValue(0.6f + 0.4f * gainOffset);
	}

	float setAndConstrain(float newValue, float min, float max) {
		if (newValue > max) {
			return max;
		}
		else if (newValue < min) {
			return min;
		}
		return newValue;
	}

	std::vector<FaderPair> pairs;
	float sampleRate;
	size_t numActivePairs;
	size_t maxNumPairs;
	float gainOffset;							// offset to manage gain difference between few voices and many voices
	float medianFreq{};
	juce::SmoothedValue<float> gain{ 0.0f };
};

