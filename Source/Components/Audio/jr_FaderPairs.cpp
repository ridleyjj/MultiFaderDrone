
#include "jr_FaderPairs.h"
#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"

//=========================================//
//************ FaderPair ******************//
//=========================================//

FaderPair::FaderPair() {};

void FaderPair::init(float _sampleRate, bool _silenced)
{
	silenced = _silenced;

	masterGain.reset(_sampleRate, rampTime);
	masterGain.setCurrentAndTargetValue(0.0f);
	masterGain.setTargetValue(silenced ? 0.0f : 1.0f);
	avgLevel.reset(_sampleRate, rampTime);
	maxLevel.reset(_sampleRate, rampTime);

	lfo.setSampleRate(_sampleRate);

	lfoBaseFreq = random.nextFloat();
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));

	resetPan(0);
	resetPan(1);

	initOscs(_sampleRate);

	isInitialised = true;
}

void FaderPair::updateSampleRate(float _sampleRate)
{
	masterGain.reset(_sampleRate, rampTime);

	lfo.setSampleRate(_sampleRate);

	for (auto& osc : oscs)
	{
		osc.setSampleRate(_sampleRate);
	}
}

void FaderPair::updateStaticSampleRate(float _sampleRate)
{
	maxLevel.reset(_sampleRate, rampTime);
	avgLevel.reset(_sampleRate, rampTime);
}

std::pair<float, float> FaderPair::process()
{
	if (oscs.size() == 0)
	{
		return out;
	}

	if (masterGain.getCurrentValue() == 0.0f && waitingToRestart)
	{
		waitingToRestart = false;
		start();
	}

	// process next sampleOut
	delta = processLevels(); // avgLevel gets incremented in here

	// reset sample out
	out.first = 0.0f;
	out.second = 0.0f;

	// raw values before panning
	float osc1RawOut = oscs.at(0).process() * (avgLevel.getCurrentValue() + delta);
	float osc2RawOut = oscs.at(1).process() * (avgLevel.getCurrentValue() - delta);

	// add panned signals to out pair
	out.first += osc1RawOut * (1.0f - pan.at(0));
	out.second += osc1RawOut * (pan.at(0));
	out.first += osc2RawOut * (1.0f - pan.at(1));
	out.second += osc2RawOut * (pan.at(1));

	// apply master gain
	masterGain.getNextValue();
	out.first *= masterGain.getCurrentValue();
	out.second *= masterGain.getCurrentValue();

	return out;
}

void FaderPair::silence()
{
	if (!silenced)
	{
		masterGain.setTargetValue(0);
		silenced = true;
	}
	waitingToRestart = false;
}

void FaderPair::start()
{
	if (masterGain.getCurrentValue() != 0.0f && silenced)
	{
		// if the voice is silenced but has not yet been faded completely, wait
		waitingToRestart = true;
	}
	else if (masterGain.getCurrentValue() == 0.0f)
	{
		// if voice is already silenced reset it and start
		resetFrequencies();
		masterGain.setTargetValue(1.0f);
		silenced = false;
	}
	// if voice is not currently silenced do nothing
}

void FaderPair::setMaxLevel(float _maxLevel)
{
	maxLevel.setTargetValue(_maxLevel);
	avgLevel.setTargetValue(_maxLevel / 2.0f);
}

void FaderPair::updateLfoFreq()
{
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));
}

void FaderPair::setStereoWidth(float width)
{
	FaderPair::stereoWidth = width;
}

void FaderPair::setLfoRate(float _rate)
{
	FaderPair::lfoRate = jr::Utils::constrainFloat(_rate);
}

bool FaderPair::getIsInitialised()
{
	return isInitialised;
}

void FaderPair::initOscs(float _sampleRate)
{
	if (oscs.size() != 2)
	{
		for (int i{}; i < 2; i++)
		{
			SineOsc osc = SineOsc();
			osc.setSampleRate(_sampleRate);
			osc.setFrequency(FaderPair::getRandomOscFrequency());
			oscs.push_back(osc);
		}
	}
}

void FaderPair::resetFrequencies()
{
	for (auto& osc : oscs)
	{
		osc.setFrequency(FaderPair::getRandomOscFrequency());
	}
	lfoBaseFreq = random.nextFloat();
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));
}

void FaderPair::resetOsc(int index)
{
	if (index < 0 || index >= oscs.size())
	{
		return;
	}
	oscs.at(index).setFrequency(FaderPair::getRandomOscFrequency());
}

void FaderPair::resetPan(int index)
{
	if (index < 0 || index >= oscs.size())
	{
		return;
	}
	pan.at(index) = 0.5f + (random.nextFloat() - 0.5f) * FaderPair::stereoWidth; // set to 0.5f +/- 0.5f at max or 0.0f at min
}

float FaderPair::getLfoFreqFromScale(float scale)
{
	scale = jr::Utils::constrainFloat(scale);
	return minLfoFreq + ((maxLfoFreq * lfoRate) - minLfoFreq) * scale;
}

float FaderPair::processLevels()
{
	float lfoVal = lfo.process();
	if (lfoVal >= 1.0f)
	{
		resetOsc(1);
		resetPan(1);
	}
	else if (lfoVal <= -1.0f)
	{
		resetOsc(0);
		resetPan(0);
	}
	return lfoVal * avgLevel.getCurrentValue();
}

void FaderPair::processStaticLevels()
{
	maxLevel.getNextValue();
	avgLevel.getNextValue();
	normalRatio = 1.0f / maxLevel.getCurrentValue();
}

float FaderPair::getRandomOscFrequency()
{
	return (FaderPair::random.nextFloat() * (FaderPair::maxOscFreq - FaderPair::minOscFreq)) + FaderPair::minOscFreq;
}

//=========================================//
//************ FaderPairs *****************//
//=========================================//

void FaderPairs::init(size_t numPairs, float _sampleRate, size_t maxNumPairs)
{
	if (_sampleRate <= 0.0f)
	{
		return;
	}

	sampleRate = _sampleRate;

	gain.reset(sampleRate, 0.1f);

	FaderPair::updateStaticSampleRate(sampleRate);

	if (_pairs.size() == 0)
	{
		// first time only

		const float maxLevel = 0.5f / (float)numPairs;

		FaderPair::setMaxLevel(maxLevel);

		for (int i{}; i < maxNumPairs; i++)
		{
			_pairs.push_back(FaderPair());
			_pairs.at(i).init(sampleRate, i >= numPairs);
		}

		numActivePairs = numPairs;

		setGainOffset();
	}
	else
	{
		// every time sampleRate changes

		for (auto& pair : _pairs)
		{
			pair.updateSampleRate(sampleRate);
		}
	}
}

std::pair<float, float> FaderPairs::process()
{
	out.first = 0.0f;
	out.second = 0.0f;

	if (!isInitialised)
	{
		bool allFinished = true;
		for (auto pair : _pairs)
		{
			if (!pair.getIsInitialised())
			{
				allFinished = false;
				break;
			}
		}
		isInitialised = allFinished;
		if (!isInitialised)
		{
			return out;
		}
	}

	float sampleOut{};

	FaderPair::processStaticLevels();

	for (auto& pair : _pairs)
	{
		auto pairOut = pair.process();
		out.first += pairOut.first;
		out.second += pairOut.second;
	}

	gain.getNextValue();

	out.first *= gain.getCurrentValue();
	out.second *= gain.getCurrentValue();

	return out;
}

void FaderPairs::setNumPairs(int numPairs)
{
	if (_pairs.size() == 0)
	{
		return;
	}

	if (numPairs == numActivePairs)
	{
		return;
	}
	if (numPairs < 0)
	{
		numPairs = 0;
	}
	else if (numPairs > _pairs.size())
	{
		numPairs = _pairs.size();
	}
	
	float maxLevel = 1.0f / (float)numPairs;
	FaderPair::setMaxLevel(maxLevel);

	if (numPairs < numActivePairs) // silencing n pairs
	{
		for (int i{ numActivePairs - 1 }; i >= numPairs; i--)
		{
			_pairs.at(i).silence();
		}
	}
	else if (numPairs > numActivePairs) // starting n pairs
	{
		for (int i{ numActivePairs }; i < numPairs; i++)
		{
			_pairs.at(i).start();
		}
	}

	numActivePairs = numPairs;
	setGainOffset();
}

void FaderPairs::setLfoRate(float _rate)
{
	FaderPair::setLfoRate(_rate);
	for (auto& pair : _pairs)
	{
		pair.updateLfoFreq();
	}
}

void FaderPairs::setStereoWidth(float width)
{
	FaderPair::setStereoWidth(jr::Utils::constrainFloat(width));
}

void FaderPairs::setGainOffset()
{
	auto g1 = 0.3f + 0.2f * jr::Utils::constrainFloat((numActivePairs - 1) / 7.0f); // ramp volume from 0.3 to 0.5 between 1 pair and 7

	gainOffset = jr::Utils::constrainFloat((numActivePairs - 10) / 40.0f); // used to ramp volume from 0.5 to 1.0 between 10 and 40 voices

	gain.setTargetValue(g1 + 0.5f * gainOffset);
}